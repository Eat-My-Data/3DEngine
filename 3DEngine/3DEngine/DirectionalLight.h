#pragma once
#include "Drawable.h"
#include "ConstantBuffers.h"

class DirectionalLight : public Drawable
{
public:
	DirectionalLight( Graphics& gfx );
	void SetDirection( DirectX::XMFLOAT3 direction ) noexcept;
	void SpawnControlWindow( Graphics& gfx ) noexcept;
	void DrawDirLight( Graphics& gfx, DirectX::XMFLOAT3 camPos );
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
private:
	struct CamPosBuffer
	{
		DirectX::XMFLOAT3 camPos;
		float padding2;
	} cambuf;
	std::shared_ptr<Bind::PixelConstantBuffer<CamPosBuffer>> pcs;
	struct LightBufferType
	{
		DirectX::XMFLOAT3 lightDirection = { 0.0f, -1.0f, -1.0f };
		float padding;
		DirectX::XMMATRIX cameraMatrix;
		DirectX::XMMATRIX projInvMatrix;
	} lbuf;

	std::shared_ptr<Bind::PixelConstantBuffer<LightBufferType>> pcs2;
	ID3D11Buffer* m_lightBuffer;
};
