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

	// need to make into quad
	auto model = Sphere::Make();
	const auto geometryTag = "dirlight.";
	AddBind( VertexBuffer::Resolve( gfx, geometryTag, model.vertices ) );
	AddBind( IndexBuffer::Resolve( gfx, geometryTag, model.indices ) );

	auto pvs = VertexShader::Resolve( gfx, "LightVS.cso" );

	auto pvsbc = pvs->GetBytecode();
	AddBind( std::move( pvs ) );

	AddBind( PixelShader::Resolve( gfx, "LightPS.cso" ) );

	AddBind( Sampler::Resolve( gfx ) );

	Dvtx::VertexBuffer vbuf( std::move(
		Dvtx::VertexLayout{}
		.Append( Dvtx::VertexLayout::Position3D )
		.Append( Dvtx::VertexLayout::Texture2D )
	) );


	struct LightBufferType
	{
		DirectX::XMFLOAT3 lightDirection = { 10.0f, -10.0f, 0.0f };
		float padding;
	} lbuf;

	// Create the constant buffer pointer so we can access the pixel shader constant buffer from within this class.
	AddBind( PixelConstantBuffer<LightBufferType>::Resolve( gfx,lbuf,1u ) );

	AddBind( InputLayout::Resolve( gfx, vbuf.GetLayout(), pvsbc ) );

	AddBind( Topology::Resolve( gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ) );

	//AddBind( std::make_shared<TransformCbuf>( gfx, *this ) );

	//AddBind( Blender::Resolve( gfx, false ) );

	AddBind( Rasterizer::Resolve( gfx, false ) );
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
