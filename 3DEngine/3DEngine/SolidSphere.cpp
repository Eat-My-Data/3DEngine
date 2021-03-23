#include "SolidSphere.h"
#include "BindableCommon.h"
#include "GraphicsThrowMacros.h"
#include "Vertex.h"
#include "Sphere.h"
#include "Vertex.h"

SolidSphere::SolidSphere( Graphics& gfx,float radius )
{
	using namespace Bind;
	namespace dx = DirectX;
	
	auto model = Sphere::Make();
	model.Transform( dx::XMMatrixScaling( radius,radius,radius ) );
	const auto geometryTag = "$sphere." + std::to_string( radius );
	AddBind( VertexBuffer::Resolve( gfx,geometryTag,model.vertices ) );
	AddBind( IndexBuffer::Resolve( gfx,geometryTag,model.indices ) );

	auto pvs = VertexShader::Resolve( gfx,"LightVS.cso" );
	auto pvsbc = pvs->GetBytecode();
	AddBind( std::move( pvs ) );

	AddBind( PixelShader::Resolve( gfx,"LightPS.cso" ) );

	struct PSColorConstant
	{
		dx::XMFLOAT3 color = { 1.0f,1.0f,1.0f };
		float padding;
	} colorConst;
	AddBind( PixelConstantBuffer<PSColorConstant>::Resolve( gfx,colorConst,1u ) );

	AddBind( Sampler::Resolve( gfx ) );

	Dvtx::VertexBuffer vbuf( std::move(
		Dvtx::VertexLayout{}
		.Append( Dvtx::VertexLayout::Position3D )
		.Append( Dvtx::VertexLayout::Texture2D )
	) );

	D3D11_BUFFER_DESC lightBufferDesc;
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof( LightBufferType );
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the pixel shader constant buffer from within this class.
	HRESULT result = gfx.GetDevice()->CreateBuffer( &lightBufferDesc, NULL, &m_lightBuffer );
	if ( FAILED( result ) )
	{
		throw ChiliException( __LINE__,__FILE__ );
	}

	AddBind( InputLayout::Resolve( gfx,vbuf.GetLayout(),pvsbc ) );

	AddBind( Topology::Resolve( gfx,D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ) );
	
	AddBind( std::make_shared<TransformCbuf>( gfx,*this ) );

	AddBind( Blender::Resolve( gfx,false ) );

	AddBind( Rasterizer::Resolve( gfx,false ) );
}

void SolidSphere::SetPos( DirectX::XMFLOAT3 pos ) noexcept
{
	this->pos = pos;
}

DirectX::XMMATRIX SolidSphere::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixTranslation( pos.x,pos.y,pos.z );
}
