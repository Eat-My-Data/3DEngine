
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

	struct PSColorConstant
	{
		dx::XMFLOAT3 color = { 1.0f,1.0f,1.0f };
		float padding;
	} colorConst;
	AddBind( PixelConstantBuffer<PSColorConstant>::Resolve( gfx, colorConst, 1u ) );

	Dvtx::VertexBuffer vbuf( std::move(
		Dvtx::VertexLayout{}
		.Append( Dvtx::VertexLayout::Position3D )
	) );
	AddBind( InputLayout::Resolve( gfx, vbuf.GetLayout(), pvsbc ) );

	AddBind( Topology::Resolve( gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ) );

	AddBind( Rasterizer::Resolve( gfx, false ) );
}

void PointLight::SetDirection( DirectX::XMFLOAT3 pos ) noexcept
{
	this->pos = pos;
}

DirectX::XMMATRIX PointLight::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixTranslation( 1.0f, 1.0f, 1.0f );
}