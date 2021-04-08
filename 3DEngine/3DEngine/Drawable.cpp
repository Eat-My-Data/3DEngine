#include "Drawable.h"
#include "GraphicsThrowMacros.h"
#include "IndexBuffer.h"
#include <cassert>

using namespace Bind;

void Drawable::Draw( Graphics& gfx ) const noxnd
{
	gfx.GetContext()->RSSetState( NULL );
	const float blendFactor[4] = { 0.f, 0.f, 0.f, 0.f };
	gfx.GetContext()->OMSetBlendState( NULL, blendFactor, 0xFFFFFFFF );

	for( auto& b : binds )
	{
		b->Bind( gfx );
	}
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
