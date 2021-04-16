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

	pcs = PixelConstantBuffer<CamPosBuffer>::Resolve( gfx, cambuf, 1u );
	AddBind( pcs );

	pcs2 = PixelConstantBuffer<LightBufferType>::Resolve( gfx, lbuf, 0u );
	AddBind( pcs2 );

	AddBind( Topology::Resolve( gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ) );

	AddBind( Rasterizer::Resolve( gfx, true ) );
}

void DirectionalLight::SetDirection( DirectX::XMFLOAT3 direction ) noexcept
{
	// don't need to change directional light's direction
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
	// set render target
	gfx.GetContext()->OMSetRenderTargets( 1, gfx.GetLightBuffer(), NULL );

	// set blend state for light values
	gfx.GetContext()->RSSetState( gfx.GetRasterizerState() );
	const float blendFactor[4] = { 0.f, 0.f, 0.f, 0.f };
	gfx.GetContext()->OMSetBlendState( gfx.GetBlendState(), blendFactor, 0xFFFFFFFF );

	// set shader resources
	gfx.GetContext()->PSSetShaderResources( 0, 3, gfx.GetShaderResources() );
	gfx.GetContext()->PSSetShaderResources( 3, 1, gfx.GetDepthResource() );
	
	// update camera position
	cambuf.camPos = camPos;
	pcs->Update( gfx, cambuf );

	// get camera matrix from view matrix
	DirectX::XMVECTOR determinant = DirectX::XMMatrixDeterminant( gfx.GetCamera() );
	DirectX::XMMATRIX cameraMatrix = DirectX::XMMatrixInverse( &determinant, gfx.GetCamera() );
	lbuf.cameraMatrix = cameraMatrix;

	// get inverse of the projection matrix
	DirectX::XMVECTOR determinant2 = DirectX::XMMatrixDeterminant( gfx.GetProjection() );
	DirectX::XMMATRIX viewMatrix2 = DirectX::XMMatrixInverse( &determinant2, gfx.GetProjection() );
	lbuf.projInvMatrix = viewMatrix2;
	pcs2->Update( gfx, lbuf );

	// bindables
	for ( auto& b : binds )
	{
		b->Bind( gfx );
	}

	// draw
	gfx.GetContext()->Draw( 3, 0 );

	// clear shader resources
	ID3D11ShaderResourceView* null[] = { nullptr, nullptr, nullptr, nullptr };
	gfx.GetContext()->PSSetShaderResources( 0, 4, null );
}