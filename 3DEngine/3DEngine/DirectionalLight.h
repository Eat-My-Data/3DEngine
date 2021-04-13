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
#if 1
	struct LightBufferType
	{
		DirectX::XMFLOAT3 lightDirection = { 0.0f, -1.0f, -1.0f };
		float padding;
		DirectX::XMMATRIX mvpMatrix;
		DirectX::XMMATRIX viewInvMatrix;
		//DirectX::XMFLOAT3 camPos;
		//float padding2;
	} lbuf;
#else
	struct LightBufferType
	{
		DirectX::XMFLOAT3 lightDirection = { 0.0f, -1.0f, -1.0f };
		float padding;
		DirectX::XMMATRIX projMatrixInv;
		DirectX::XMMATRIX viewMatrixInv;
		//DirectX::XMFLOAT3 camPos;
		//float padding2;
	} lbuf;
#endif
	std::shared_ptr<Bind::PixelConstantBuffer<LightBufferType>> pcs2;
	//DirectX::XMFLOAT3 lightDirection;
	ID3D11Buffer* m_lightBuffer;
};
