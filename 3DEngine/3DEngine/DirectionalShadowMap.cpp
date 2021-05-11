#include "DirectionalShadowMap.h"
#include "BindableCommon.h"
#include "TransformCbufDoubleboi.h"


DirectionalShadowMap::DirectionalShadowMap( Graphics& gfx )
{
	using namespace Bind;
	namespace dx = DirectX;

	//auto pvs = VertexShader::Resolve( gfx, "ShadowMapVS.cso" );

	//auto pvsbc = pvs->GetBytecode();
	//AddBind( std::move( pvs ) );

	//AddBind( PixelShader::Resolve( gfx, "ShadowMapPS.cso" ) ); \
		AddBind( Sampler::Resolve( gfx ) );

	pcs = PixelConstantBuffer<ShadowMapConstant>::Resolve( gfx, sbuf, 0u );
	AddBind( pcs );

	AddBind( Topology::Resolve( gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ) );
	
	AddBind( Rasterizer::Resolve( gfx, true ) );
}

void DirectionalShadowMap::Render( Graphics& gfx, Camera& cam )
{

}

DirectX::XMMATRIX DirectionalShadowMap::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixTranslation( 1.0f, 1.0f, 1.0f );
}

void DirectionalShadowMap::CreateShadowMap( Graphics& gfx, DirectX::XMFLOAT3 lightDirection )
{
	//const DirectX::XMVECTOR upBaseVector = DirectX::XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );
	//DirectX::XMMATRIX perspLH = DirectX::XMMatrixLookAtLH( m_EyePosition, m_LookAt, upBaseVector );
	//DirectX::XMMATRIX orthoLH = DirectX::XMMatrixOrthographicLH( m_ViewWidth, m_ViewHeight, m_NearZ, m_FarZ );

	// set render target
	gfx.GetContext()->OMSetRenderTargets( 1, gfx.GetShadowRTV(), 0 );

	// bindables
	for ( auto& b : binds )
	{
		b->Bind( gfx );
	}

	// draw
	gfx.GetContext()->Draw( 3, 0 );

}