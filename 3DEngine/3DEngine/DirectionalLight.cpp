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

	// Create the constant buffer pointer so we can access the pixel shader constant buffer from within this class.
	AddBind( PixelConstantBuffer<LightBufferType>::Resolve( gfx,lbuf,1u ) );

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

