#pragma once
#include "Graphics.h"
#include "ConstantBuffers.h"
#include "Drawable.h"
#include "Camera.h"

class DirectionalShadowMap : public Drawable
{
public:
    DirectionalShadowMap( Graphics& gfx );
    DirectX::XMMATRIX GetTransformXM() const noexcept;
    void Render( Graphics& gfx, Camera& cam );
    void CreateShadowMap( Graphics& gfx, DirectX::XMFLOAT3 lightDirection );
private:
    struct ShadowMapConstant
    {
        float mapWidth;
        float mapHeight;
        DirectX::XMMATRIX lightViewProjection;
    } sbuf;
    std::shared_ptr<Bind::PixelConstantBuffer<ShadowMapConstant>> pcs;

    DirectX::XMVECTOR m_EyePosition;
    DirectX::XMVECTOR m_LookAt;
    float m_NearZ = 0.1;
    float m_FarZ = 200;
    float m_ViewWidth = 1;
    float m_ViewHeight = 1;
    float m_TextureWidth;
    float m_TextureHeight;
};