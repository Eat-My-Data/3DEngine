#include "ShaderOps.hlsl"
#include "LightVectorData.hlsl"

Texture2D colorTexture : register(t0);
Texture2D normalTexture : register(t1);
Texture2D specularTexture : register(t2);
Texture2D depthTexture : register(t3);

SamplerState SampleTypePoint : register(s0);

cbuffer CBuf : register(b0)
{
    float3 color;
    float padding;
    row_major float4x4 cameraMatrix;
    row_major float4x4 projInvMatrix;
};

cbuffer CBuf : register(b1)
{
    float3 lightPosition;
    float padding2;
};

cbuffer CamPosBuffer : register(b2)
{
    float3 camPos;
    float padding3;
};

float4 main(float4 position : SV_POSITION) : SV_TARGET
{
    float2 screenPos;
    screenPos.x = position.x / 1280.0f;
    screenPos.y = position.y / 720.0f;

    float4 colors = colorTexture.Load(int3(position.xy, 0));
    float4 normals = normalTexture.Load(int3(position.xy, 0));
    float4 specular = specularTexture.Load(int3(position.xy, 0));

    float depthSample = depthTexture.Load(int3(position.xy, 0)).r; //* 2.0 - 1.0;
    float clipX = (screenPos.x * 2.0) - 1.0;
    float clipY = (screenPos.y * 2.0) - 1.0;
    clipY = -clipY;
    //clipX = -clipX;
   
    normals = (normals * 2.0) - 1.0;
    
    //float4 lightPos = mul(float4(lightPosition, 1), camMatrix);
    //lightPos /= lightPos.w;
    
    float4 worldDepth = float4(clipX, clipY, depthSample, 1.0);
    float4 worldPosition = mul(worldDepth, projInvMatrix);
    worldPosition /= worldPosition.w;
    
    //float4 newLight = mul(float4(lightPosition, 1), mvpMatrix);
    float4 worldSpacePos = mul(worldPosition, cameraMatrix);
    const LightVectorData lv = CalculateLightVectorData(lightPosition, worldSpacePos.xyz);
    
    //float3 vToL = lightPosition.xyz - worldSpacePos.xyz;
    //float len = length(vToL);
    
    if ( lv.distToL > 15 )
    {
        return float4(0, 0, 0, 0);
    }
    
    float attConst = 0.0f;
    float attLin = 0.045f;
    float attQuad = 0.0075f;
    float att = Attenuate(attConst, attLin, attQuad, lv.distToL);

    //float diffuseIntensity = saturate(dot(normalize(normals.xyz), normalize(vToL.xyz)));
    float3 diffuseColor = Diffuse(colors.rgb, 1.0f, att, lv.dirToL /15.0f, normalize(normals.xyz));
    
    float3 camToFrag = worldSpacePos.xyz - camPos;
    
    float3 specularResult = Speculate(specular.xyz, 1, normalize(normals.xyz), lv.dirToL / 15.0f, camToFrag, att, 128);
    //return float4(1, 1, 1, 1);
    
    return float4(color, 1) * (float4(diffuseColor, 1) + float4(specularResult, 1.0f));
    
}