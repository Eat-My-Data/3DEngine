
#include "PointLight.h"
#include "BindableCommon.h"
#include "GraphicsThrowMacros.h"
#include "Vertex.h"
#include "Sphere.h"

PointLight::PointLight( Graphics& gfx, float radius )
{
	using namespace Bind;
	namespace dx = DirectX;

	auto model = Sphere::Make();
	model.Transform( dx::XMMatrixScaling( radius, radius, radius ) );
	const auto geometryTag = "$sphere." + std::to_string( radius );
	AddBind( VertexBuffer::Resolve( gfx, geometryTag, model.vertices ) );
	AddBind( IndexBuffer::Resolve( gfx, geometryTag, model.indices ) );

	auto pvs = VertexShader::Resolve( gfx, "PointLightVS.cso" );
	auto pvsbc = pvs->GetBytecode();
	AddBind( std::move( pvs ) );

	AddBind( PixelShader::Resolve( gfx, "PointLightPS.cso" ) );

	AddBind( Sampler::Resolve( gfx ) );

	colorConst.radius = radius;
	pcs = PixelConstantBuffer<PSColorConstant>::Resolve( gfx, colorConst, 0u );
	AddBind(  pcs );

	pcs2 = PixelConstantBuffer<PSPositionConstant>::Resolve( gfx, posConst, 1u );
	AddBind( pcs2 );

	pcs3 = PixelConstantBuffer<CamPosBuffer>::Resolve( gfx, cambuf, 2u );
	AddBind( pcs3 );

	Dvtx::VertexBuffer vbuf( std::move(
		Dvtx::VertexLayout{}
		.Append( Dvtx::VertexLayout::Position3D )
	) );
	AddBind( InputLayout::Resolve( gfx, vbuf.GetLayout(), pvsbc ) );

	AddBind( Topology::Resolve( gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ) );

	AddBind( std::make_shared<TransformCbuf>( gfx, *this ) );

	//AddBind( Rasterizer::Resolve( gfx, true ) );
}

void PointLight::SetDirection( DirectX::XMFLOAT3 direction ) noexcept
{
	// don't need direction for point light
}

DirectX::XMMATRIX PointLight::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixTranslation( posConst.lightPosition.x, posConst.lightPosition.y, posConst.lightPosition.z );
}

void PointLight::DrawPointLight( Graphics& gfx, DirectX::FXMMATRIX view,DirectX::XMFLOAT3 camPos )
{
	// figure out if camera is inside point light
	if ( CameraIsInside( camPos ) )
	{
		// cull backface
		gfx.GetContext()->RSSetState( gfx.GetRasterizerStateInside() );
	}
	else
	{
		gfx.GetContext()->RSSetState( gfx.GetRasterizerStateOutside() );
	}

	// get camera matrix from view matrix
	DirectX::XMVECTOR determinant = DirectX::XMMatrixDeterminant( gfx.GetCamera() );
	DirectX::XMMATRIX cameraMatrix = DirectX::XMMatrixInverse( &determinant, gfx.GetCamera() );
	colorConst.cameraMatrix = cameraMatrix;

	// get inverse of the projection matrix
	DirectX::XMVECTOR determinant2 = DirectX::XMMatrixDeterminant( gfx.GetProjection() );
	DirectX::XMMATRIX viewMatrix2 = DirectX::XMMatrixInverse( &determinant2, gfx.GetProjection() );
	colorConst.projInvMatrix = viewMatrix2;
	pcs->Update( gfx, colorConst );

	// update light position
	pcs2->Update( gfx, posConst );
	
	// update camera position
	cambuf.camPos = camPos;
	pcs3->Update( gfx, cambuf );

	// bindables
	for ( auto& b : binds )
	{
		b->Bind( gfx );
	}

	// draw
	gfx.DrawIndexed( pIndexBuffer->GetCount() );

	// clear shader resources
	ID3D11ShaderResourceView* null[] = { nullptr, nullptr, nullptr, nullptr };
	gfx.GetContext()->PSSetShaderResources( 0, 4, null );
}

void PointLight::SetPos( DirectX::XMFLOAT3 vec )
{
	posConst.lightPosition.x += vec.x;
	posConst.lightPosition.y += vec.y;
	posConst.lightPosition.z += vec.z;
}

bool PointLight::CameraIsInside( DirectX::XMFLOAT3 camPos )
{
	float distFromCenterX = posConst.lightPosition.x - camPos.x;
	float distFromCenterY = posConst.lightPosition.y - camPos.y;
	float distFromCenterZ = posConst.lightPosition.z - camPos.z;
	float xSq = distFromCenterX * distFromCenterX;
	float ySq = distFromCenterY * distFromCenterY;
	float zSq = distFromCenterZ * distFromCenterZ;
	float distSq = xSq + ySq + zSq;

	float radiusSq = (colorConst.radius + 0.5f) * (colorConst.radius + 0.5f);

	return distSq <= radiusSq;
}


