#pragma once
#include "Graphics.h"
#include "SolidSphere.h"
#include "ConstantBuffers.h"
#include "ConditionalNoexcept.h"

class PointLight
{
public:
	PointLight( Graphics& gfx,float radius = 0.5f );						// initialize cbData, mesh, and cbuf
	void SpawnControlWindow() noexcept;										// imgui window
	void Reset() noexcept;													// resets cbData to default values
	void Draw( Graphics& gfx ) const noxnd;									// update mesh pos from imgui, call mesh's draw function
	void Bind( Graphics& gfx, DirectX::FXMMATRIX view ) const noexcept;		// set position with view transformation, update and bind cbuf
private:
	// constant buffer values
	struct PointLightCBuf
	{
		alignas(16) DirectX::XMFLOAT3 pos;									
		alignas(16) DirectX::XMFLOAT3 ambient;
		alignas(16)	DirectX::XMFLOAT3 diffuseColor;
		float diffuseIntensity = 1.0f;
		float attConst = 1.0f;
		float attLin = 0.045f;
		float attQuad = 0.0075f;
	};
private:
	PointLightCBuf cbData;													// constant buffer data
	mutable SolidSphere mesh;												// drawable 
	mutable Bind::PixelConstantBuffer<PointLightCBuf> cbuf;					// constant buffer
};