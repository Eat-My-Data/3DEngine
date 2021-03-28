#include "Drawable.h"
#include "GraphicsThrowMacros.h"
#include "IndexBuffer.h"
#include <cassert>

using namespace Bind;

void Drawable::Draw( Graphics& gfx ) const noxnd
{
	for( auto& b : binds )
	{
		b->Bind( gfx );
	}
	gfx.DrawIndexed( pIndexBuffer->GetCount() );
}

void Drawable::DrawDirLight( Graphics& gfx )
{
	gfx.GetContext()->IASetVertexBuffers( 0, 0, NULL, NULL, NULL );
	gfx.GetContext()->IASetIndexBuffer( NULL, (DXGI_FORMAT)0,0 );
	gfx.GetContext()->IASetInputLayout( NULL );
	const float color[] = { 0.07f,0.0f,0.12f };
	gfx.GetContext()->OMSetRenderTargets( 1, gfx.GetLightBuffer(), NULL );
	gfx.GetContext()->PSSetShaderResources( 0, 2, gfx.GetShaderResources() );

	for ( auto& b : binds )
	{
		b->Bind( gfx );
	}

	gfx.GetContext()->Draw( 3,0 );

	ID3D11ShaderResourceView* null[] = { nullptr, nullptr };
	gfx.GetContext()->PSSetShaderResources( 0, 2, null );
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
