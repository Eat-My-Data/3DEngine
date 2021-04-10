#include "DirectionalLight.h"
#include "Cube.h"
#include "BindableCommon.h"
#include "TransformCbufDoubleboi.h"
#include "imgui/imgui.h"
#include "Sphere.h"

DirectionalLight::DirectionalLight( Graphics& gfx )
{
	using namespace Bind;
	namespace dx = DirectX;

	auto pvs = VertexShader::Resolve( gfx, "LightVS.cso" );

	auto pvsbc = pvs->GetBytecode();
	AddBind( std::move( pvs ) );

	AddBind( PixelShader::Resolve( gfx, "LightPS.cso" ) );\
	AddBind( Sampler::Resolve( gfx ) );

#if 1
	DirectX::XMVECTOR determinant = DirectX::XMMatrixDeterminant( gfx.GetCamera() );
	DirectX::XMMATRIX viewMatrix = DirectX::XMMatrixInverse( &determinant, gfx.GetCamera() );
	lbuf.mvpMatrix = viewMatrix * gfx.GetProjection();
	DirectX::XMVECTOR determinant2 = DirectX::XMMatrixDeterminant( lbuf.mvpMatrix );
	DirectX::XMMATRIX viewMatrix2 = DirectX::XMMatrixInverse( &determinant2, lbuf.mvpMatrix );
	lbuf.mvpMatrix = DirectX::XMMatrixTranspose( viewMatrix2 );

	//pcs2->Update( gfx, lbuf );
#else
	DirectX::XMVECTOR determinant = DirectX::XMMatrixDeterminant( gfx.GetProjection() );
	DirectX::XMMATRIX projMatrixInv = DirectX::XMMatrixInverse( &determinant, gfx.GetProjection() );
	lbuf.projMatrixInv = projMatrixInv;
	DirectX::XMVECTOR determinant2 = DirectX::XMMatrixDeterminant( gfx.GetCamera() );
	DirectX::XMMATRIX viewMatrixInv = DirectX::XMMatrixInverse( &determinant2, gfx.GetCamera() );
	lbuf.viewMatrixInv = DirectX::XMMatrixTranspose( viewMatrixInv );
#endif

	pcs2 = PixelConstantBuffer<LightBufferType>::Resolve( gfx, lbuf, 0u );
	AddBind( pcs2 );
	pcs = PixelConstantBuffer<CamPosBuffer>::Resolve( gfx, cambuf, 1u );
	AddBind( pcs );

	AddBind( Topology::Resolve( gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ) );

	AddBind( Rasterizer::Resolve( gfx, true ) );
}

void DirectionalLight::SetDirection( DirectX::XMFLOAT3 direction ) noexcept
{
	//this->lightDirection = direction;
}

void DirectionalLight::SpawnControlWindow( Graphics& gfx ) noexcept
{
	if ( ImGui::Begin( "Directional Light" ) )
	{
		ImGui::Text( "Direction" );

	}
	ImGui::End();
}

DirectX::XMMATRIX DirectionalLight::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixTranslation( 1.0f, 1.0f, 1.0f );
}

void DirectionalLight::DrawDirLight( Graphics& gfx, DirectX::XMFLOAT3 camPos )
{
	gfx.GetContext()->OMSetRenderTargets( 1, gfx.GetLightBuffer(), NULL );

	gfx.GetContext()->RSSetState( gfx.rasterizerDR );
	const float blendFactor[4] = { 0.f, 0.f, 0.f, 0.f };
	gfx.GetContext()->OMSetBlendState( gfx.GetBlendState(), blendFactor, 0xFFFFFFFF );

	gfx.GetContext()->PSSetShaderResources( 0, 3, gfx.GetShaderResources() );
	gfx.GetContext()->PSSetShaderResources( 3, 1, gfx.GetDepthResource() );

	cambuf.camPos = camPos;
	pcs->Update( gfx, cambuf );
#if 1
	DirectX::XMVECTOR determinant = DirectX::XMMatrixDeterminant( gfx.GetCamera() );
	DirectX::XMMATRIX viewMatrix = DirectX::XMMatrixInverse( &determinant, gfx.GetCamera() );
	lbuf.mvpMatrix = viewMatrix * gfx.GetProjection();
	DirectX::XMVECTOR determinant2 = DirectX::XMMatrixDeterminant( lbuf.mvpMatrix );
	DirectX::XMMATRIX viewMatrix2 = DirectX::XMMatrixInverse( &determinant2, lbuf.mvpMatrix );
	lbuf.mvpMatrix = DirectX::XMMatrixTranspose( viewMatrix2 );
	pcs2->Update( gfx, lbuf );
#else
	DirectX::XMVECTOR determinant = DirectX::XMMatrixDeterminant( gfx.GetProjection() );
	DirectX::XMMATRIX projMatrixInv = DirectX::XMMatrixInverse( &determinant, gfx.GetProjection() );
	lbuf.projMatrixInv = projMatrixInv;
	DirectX::XMVECTOR determinant2 = DirectX::XMMatrixDeterminant( gfx.GetCamera() );
	DirectX::XMMATRIX viewMatrixInv = DirectX::XMMatrixInverse( &determinant2, gfx.GetCamera() );
	lbuf.viewMatrixInv = DirectX::XMMatrixTranspose( viewMatrixInv );
	pcs2->Update( gfx, lbuf );
#endif


	for ( auto& b : binds )
	{
		b->Bind( gfx );
	}

	gfx.GetContext()->Draw( 3, 0 );

	ID3D11ShaderResourceView* null[] = { nullptr, nullptr, nullptr, nullptr };
	gfx.GetContext()->PSSetShaderResources( 0, 4, null );
}