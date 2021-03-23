#pragma once
#include "Drawable.h"
#include <DirectXMath.h>


class SolidSphere : public Drawable
{
public:
	SolidSphere( Graphics& gfx,float radius );
	void SetPos( DirectX::XMFLOAT3 pos ) noexcept;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
private:
	DirectX::XMFLOAT3 pos = { 1.0f,1.0f,1.0f };
	struct LightBufferType
	{
		DirectX::XMFLOAT3 lightDirection;
		float padding;
	};
	ID3D11Buffer* m_lightBuffer;
};