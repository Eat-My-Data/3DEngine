#pragma once
#include "Graphics.h"
#include <DirectXMath.h>
#include "ConditionalNoexcept.h"

namespace Bind
{
	class Bindable;
	class IndexBuffer;
}

class Drawable
{
	template<class T>
	friend class DrawableBase;											// drawable base for common attributes
public:
	Drawable() = default;
	Drawable( const Drawable& ) = delete;
	virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;		// virtual getter for transform matrix
	void Draw( Graphics& gfx ) const noxnd;								// binds all attributes to model, and calls DrawIndexed
	virtual void Update( float dt ) noexcept							// virtual update for drawable
	{}
	virtual ~Drawable() = default;										// esure destruction of derived class
protected:
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
	void AddBind( std::unique_ptr<Bind::Bindable> bind ) noxnd;				// add drawable specific bind
	void AddIndexBuffer( std::unique_ptr<Bind::IndexBuffer> ibuf ) noxnd;	// add index buffer
private:
	virtual const std::vector<std::unique_ptr<Bind::Bindable>>& GetStaticBinds() const noexcept = 0;	// returns common drawable attributes
private:
	const Bind::IndexBuffer* pIndexBuffer = nullptr;						// pointer to index buffer
	std::vector<std::unique_ptr<Bind::Bindable>> binds;						// container of bindables
};
