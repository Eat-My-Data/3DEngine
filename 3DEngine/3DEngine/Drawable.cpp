#include "Drawable.h"
#include "GraphicsThrowMacros.h"
#include "IndexBuffer.h"
#include "VertexShader.h"
#include <cassert>

using namespace Bind;

void Drawable::Draw( Graphics& gfx ) const noxnd
{
	// set blend state to default for geometry
	gfx.GetContext()->RSSetState( NULL );
	const float blendFactor[4] = { 0.f, 0.f, 0.f, 0.f };
	gfx.GetContext()->OMSetBlendState( NULL, blendFactor, 0xFFFFFFFF );

	// bindables
	for( auto& b : binds )
	{
		b->Bind( gfx );
	}

	// draw
	gfx.DrawIndexed( pIndexBuffer->GetCount() );
}

void Drawable::DrawModelDepth( Graphics& gfx ) const noxnd
{
	// set blend state to default for geometry
	gfx.GetContext()->RSSetState( NULL );
	const float blendFactor[4] = { 0.f, 0.f, 0.f, 0.f };
	gfx.GetContext()->OMSetBlendState( NULL, blendFactor, 0xFFFFFFFF );

	// bindables
	for ( auto& b : binds )
	{
		b->Bind( gfx );
	}

	// change what vertex/pixel shaders are being used
	//auto pvs = VertexShader::Resolve( gfx, "ShadowMapVS.cso" );
	//auto pvsbc = pvs->GetBytecode();
	//pvs->Bind( gfx );
	gfx.GetContext()->PSSetShader( nullptr, nullptr, 0u );

	// draw
	gfx.DrawIndexed( pIndexBuffer->GetCount() );
}

void Drawable::AddBind( std::shared_ptr<Bind::Bindable> bind ) noxnd
{
	if (typeid(*bind) == typeid(IndexBuffer))
	{
		assert( "Binding multiple index buffers not allowed" && pIndexBuffer == nullptr );
		pIndexBuffer = &static_cast<IndexBuffer&>(*bind);
	}
	binds.push_back( std::move( bind ) );
}
