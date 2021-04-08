
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


	DirectX::XMVECTOR determinant = DirectX::XMMatrixDeterminant( gfx.GetCamera() );
	DirectX::XMMATRIX viewMatrix = DirectX::XMMatrixInverse( &determinant, gfx.GetCamera() );
	colorConst.mvpMatrix = viewMatrix * gfx.GetProjection();
	DirectX::XMVECTOR determinant2 = DirectX::XMMatrixDeterminant( colorConst.mvpMatrix );
	DirectX::XMMATRIX viewMatrix2 = DirectX::XMMatrixInverse( &determinant2, colorConst.mvpMatrix );
	colorConst.mvpMatrix = DirectX::XMMatrixTranspose( viewMatrix2 );

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

	AddBind( Rasterizer::Resolve( gfx, false ) );
}

void PointLight::SetDirection( DirectX::XMFLOAT3 pos ) noexcept
{
	//this->pos = pos;
}

DirectX::XMMATRIX PointLight::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixTranslation( posConst.lightPosition.x, posConst.lightPosition.y, posConst.lightPosition.z );
}

void PointLight::DrawPointLight( Graphics& gfx, DirectX::FXMMATRIX view )
{
	gfx.GetContext()->OMSetRenderTargets( 1, gfx.GetLightBuffer(), NULL );
	gfx.GetContext()->PSSetShaderResources( 0, 3, gfx.GetShaderResources() );
	gfx.GetContext()->PSSetShaderResources( 3, 1, gfx.GetDepthResource() );

	DirectX::XMVECTOR determinant = DirectX::XMMatrixDeterminant( gfx.GetCamera() );
	DirectX::XMMATRIX viewMatrix = DirectX::XMMatrixInverse( &determinant, gfx.GetCamera() );
	colorConst.mvpMatrix = viewMatrix * gfx.GetProjection();
	DirectX::XMVECTOR determinant2 = DirectX::XMMatrixDeterminant( colorConst.mvpMatrix );
	DirectX::XMMATRIX viewMatrix2 = DirectX::XMMatrixInverse( &determinant2, colorConst.mvpMatrix );
	colorConst.mvpMatrix = DirectX::XMMatrixTranspose( viewMatrix2 );
	pcs->Update( gfx, colorConst );

	auto dataCopy = posConst;
	float temp = dataCopy.lightPosition.x;
	dataCopy.lightPosition.x = dataCopy.lightPosition.z;
	dataCopy.lightPosition.z = temp;
	const auto pos = DirectX::XMLoadFloat3( &dataCopy.lightPosition );
	DirectX::XMStoreFloat3( &dataCopy.lightPosition, DirectX::XMVector3Transform( pos, view ) );
	pcs2->Update( gfx, dataCopy );

	cambuf.camPos.x = gfx.GetCamera().r[3].m128_f32[0];
	cambuf.camPos.y = gfx.GetCamera().r[3].m128_f32[1];
	cambuf.camPos.z = gfx.GetCamera().r[3].m128_f32[2];
	pcs3->Update( gfx, cambuf );

	for ( auto& b : binds )
	{
		b->Bind( gfx );
	}

	gfx.DrawIndexed( pIndexBuffer->GetCount() );

	ID3D11ShaderResourceView* null[] = { nullptr, nullptr, nullptr, nullptr };
	gfx.GetContext()->PSSetShaderResources( 0, 4, null );
}

void PointLight::SetPos( DirectX::XMFLOAT3 vec )
{
	posConst.lightPosition.x += vec.x;
	posConst.lightPosition.y += vec.y;
	posConst.lightPosition.z += vec.z;
}
