#pragma once
#include "Graphics.h"
#include "ConditionalNoexcept.h"
#include <string>

class Drawable;
class TechniqueProbe;

namespace Bind
{
	class Bindable
	{
	public:
		virtual void Bind( Graphics& gfx ) noexcept = 0;						// binds attributes to graphics object
		virtual void InitializeParentReference( const Drawable& ) noexcept
		{}
		virtual void Accept( TechniqueProbe& )
		{}
		virtual std::string GetUID() const noexcept
		{
			assert( false );
			return "";
		}
		virtual ~Bindable() = default;
	protected:
		static ID3D11DeviceContext* GetContext( Graphics& gfx ) noexcept;		// access to device context
		static ID3D11Device* GetDevice( Graphics& gfx ) noexcept;				// access to graphics device 
		static DxgiInfoManager& GetInfoManager( Graphics& gfx );			// access to infomanager
	};
}
