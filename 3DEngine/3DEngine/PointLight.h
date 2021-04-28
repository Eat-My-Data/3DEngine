#pragma once
#include "Drawable.h"
#include "ConstantBuffers.h"

class PointLight : public Drawable
{
public:
	PointLight( Graphics& gfx,float radius );
	void SetDirection( DirectX::XMFLOAT3 direction ) noexcept;
	void SpawnControlWindow( Graphics& gfx ) noexcept;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
	void DrawPointLight( Graphics& gfx, DirectX::FXMMATRIX view, DirectX::XMFLOAT3 camPos );
	void SetPos( DirectX::XMFLOAT3 vec );
	bool CameraIsInside( DirectX::XMFLOAT3 camPos );
private:
	struct PSColorConstant
	{
		DirectX::XMFLOAT3 color = { 1.0f,1.0f,1.0f };
		float radius;

		float attConst = 0.0f;
		float attLin = 0.045f;
		float attQuad = 0.0075f;
		float specularPower = 128.0f;

		float diffuseIntensity = 1.0f;
		float specularIntensity = 1.0f;

		DirectX::XMMATRIX cameraMatrix;
		DirectX::XMMATRIX projInvMatrix;
	} colorConst;
	std::shared_ptr<Bind::PixelConstantBuffer<PSColorConstant>> pcs;

	struct PSPositionConstant
	{
		DirectX::XMFLOAT3 lightPosition = { 1.0f,1.0f,1.0f };
		float padding;
	} posConst;
	std::shared_ptr<Bind::PixelConstantBuffer<PSPositionConstant>> pcs2;

	struct CamPosBuffer
	{
		DirectX::XMFLOAT3 camPos;
		float padding2;
	} cambuf;
	std::shared_ptr<Bind::PixelConstantBuffer<CamPosBuffer>> pcs3;

	Microsoft::WRL::ComPtr<ID3D11PixelShader> pPixelShader;
};
