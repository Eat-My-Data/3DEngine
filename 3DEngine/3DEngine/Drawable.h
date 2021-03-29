#pragma once
#include "Graphics.h"
#include <DirectXMath.h>
#include "ConditionalNoexcept.h"
#include <memory>

namespace Bind
{
	class Bindable;
	class IndexBuffer;
}

class Drawable
{
public:
	Drawable() = default;
	Drawable( const Drawable& ) = delete;
	virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;		// virtual getter for transform matrix
	void Draw( Graphics& gfx ) const noxnd;								// binds all attributes to model, and calls DrawIndexed
	void DrawDirLight( Graphics& gfx );
	void DrawPointLight( Graphics& gfx );
	virtual ~Drawable() = default;										// esure destruction of derived class
	template<class T>
	T* QueryBindable() noexcept											// find bindable of type T
	{
		for( auto& pb : binds )
		{
			if( auto pt = dynamic_cast<T*>(pb.get()) )
			{
				return pt;
			}
		}
		return nullptr;
	}
protected:
	void AddBind( std::shared_ptr<Bind::Bindable> bind ) noxnd;				// add drawable specific bind
private:
	const Bind::IndexBuffer* pIndexBuffer = nullptr;						// pointer to index buffer
	std::vector<std::shared_ptr<Bind::Bindable>> binds;						// container of bindables
};
