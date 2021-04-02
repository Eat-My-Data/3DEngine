#include "ShaderOps.hlsl"

Texture2D colorTexture : register(t0);
Texture2D normalTexture : register(t1);
Texture2D specularTexture : register(t2);
Texture2D depthTexture : register(t3);

SamplerState SampleTypePoint : register(s0);

cbuffer CBuf : register(b0)
{
    float4 color;
    float4x4 mvpMatrix;
};

cbuffer CBuf : register(b1)
{
    float4 lightPosition;
};

cbuffer CamPosBuffer : register(b2)
{
    float3 camPos;
    float padding2;
};

float4 main(float4 position : SV_POSITION) : SV_TARGET
{
    float2 screenPos;
    screenPos.x = position.x / 1280;
    screenPos.y = position.y / 720;

    float4 colors = colorTexture.Sample(SampleTypePoint, screenPos);
    float4 normals = normalTexture.Sample(SampleTypePoint, screenPos);
    float4 specular = specularTexture.Sample(SampleTypePoint, screenPos);

    float depthSample = depthTexture.Sample(SampleTypePoint, screenPos).r;
    float clipX = (screenPos.x * 2.0) - 1.0;
    float clipY = (screenPos.y * 2.0) - 1.0;
    clipY = -clipY;
    clipX = -clipX;
   
    normals = (normals * 2.0) - 1.0;

    float4 worldDepth = float4(clipX, clipY, depthSample, 1.0);
    float4 worldPosition = mul(worldDepth, mvpMatrix);
    worldPosition /= worldPosition.w;
    
    float3 vToL = lightPosition.xyz - worldPosition.xyz;
    float len = length(vToL);
    float diffuseIntensity = saturate(dot(normalize(normals.xyz), normalize(vToL.xyz)));
    
    float3 camToFrag = worldPosition.xyz - camPos;
    
    //float4 outputColor = saturate(color * colors * specular.w);
    //return sampledPosition;
    float3 specularResult = Speculate(specular.xyz, 1, normals.xyz, vToL, camToFrag, .5, 128);
    
    //return colors;
    if (len > 15)
    {
        return float4(0, 0, 0, 0);

    }
    else
    {
        float4 outputColor = saturate(colors);
        return color * colors * diffuseIntensity + float4(specularResult, 1.0f);
    }
}