#pragma once
#include "Graphics.h"
#include "SolidSphere.h"
#include "ConstantBuffers.h"

class PointLight
{
public:
	PointLight( Graphics& gfx,float radius = 0.5f );
	void SpawnControlWindow() noexcept;
	void Reset() noexcept;
	void Draw( Graphics& gfx ) const noexcept( !IS_DEBUG );
	void Bind( Graphics& gfx ) const noexcept;
private:
	struct PointLightCBuf
	{
		alignas(16) DirectX::XMFLOAT3 pos;
		alignas(16) DirectX::XMFLOAT3 materialColor;
		alignas(16) DirectX::XMFLOAT3 ambient;
		alignas(16)	DirectX::XMFLOAT3 diffuseColor;
		float diffuseIntensity = 1.0f;
		float attConst = 1.0f;
		float attLin = 0.045f;
		float attQuad = 0.0075f;
	};
private:
	PointLightCBuf cbData;
	mutable SolidSphere mesh;
	mutable PixelConstantBuffer<PointLightCBuf> cbuf;
};