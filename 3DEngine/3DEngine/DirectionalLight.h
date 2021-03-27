#pragma once
#include "Drawable.h"

class DirectionalLight : public Drawable
{
public:
	DirectionalLight( Graphics& gfx );
	void SetDirection( DirectX::XMFLOAT3 direction ) noexcept;
	void SpawnControlWindow( Graphics& gfx ) noexcept;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
private:
	struct LightBufferType
	{
		DirectX::XMFLOAT3 lightDirection = { 10.0f, -10.0f, 0.0f };
		float padding;
	} lbuf;
	//DirectX::XMFLOAT3 lightDirection;
	ID3D11Buffer* m_lightBuffer;
};
