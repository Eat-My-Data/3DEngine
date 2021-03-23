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
	//DirectX::XMFLOAT3 lightDirection;
	ID3D11Buffer* m_lightBuffer;
};
