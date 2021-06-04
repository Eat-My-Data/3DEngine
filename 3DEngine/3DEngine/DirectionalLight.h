#pragma once
#include "Drawable.h"
#include "ConstantBuffers.h"

class DirectionalLight : public Drawable
{
public:
	DirectionalLight( Graphics& gfx );
	void SetDirection( DirectX::XMFLOAT3 direction ) noexcept;
	void SpawnControlWindow( Graphics& gfx ) noexcept;
	void DrawDirLight( Graphics& gfx, DirectX::XMFLOAT3 camPos, DirectX::XMMATRIX orthoMatrix,DirectX::XMFLOAT3 lightPos );
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
	DirectX::XMFLOAT3 GetLightDirection() { return lbuf.lightDirection; };
private:
	struct LightBufferType
	{
		DirectX::XMFLOAT3 lightDirection = { 0.0f, -1.0f, 0.0f };
		float padding;
		float specularIntensity = 1.0f;
		float att = 0.5f;
		float specularPower = 128.0f;
		float padding2;
		DirectX::XMMATRIX cameraMatrix;
		DirectX::XMMATRIX projInvMatrix;
	} lbuf;
	std::shared_ptr<Bind::PixelConstantBuffer<LightBufferType>> pcs;
	struct CamPosBuffer
	{
		DirectX::XMFLOAT3 camPos;
		float padding3;
		DirectX::XMFLOAT3 lightPos;
		float padding4;
		DirectX::XMMATRIX lightMatrix;
	} cambuf;
	std::shared_ptr<Bind::PixelConstantBuffer<CamPosBuffer>> pcs2;
};
