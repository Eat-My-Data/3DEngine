#pragma once
#include "Drawable.h"

class PointLight : public Drawable
{
public:
	PointLight( Graphics& gfx,float radius );
	void SetDirection( DirectX::XMFLOAT3 direction ) noexcept;
	void SpawnControlWindow( Graphics& gfx ) noexcept;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
private:
	//DirectX::XMFLOAT3 lightDirection;
	ID3D11Buffer* m_lightBuffer;
	DirectX::XMFLOAT3 pos = { 1.0f,1.0f,1.0f };
};
