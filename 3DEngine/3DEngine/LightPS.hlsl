#include "ShaderOps.hlsl"

Texture2D colorTexture : register(t0);
Texture2D normalTexture : register(t1);
Texture2D specularTexture : register(t2);
Texture2D depthTexture : register(t3);

SamplerState SampleTypePoint : register(s0);

#if 1
cbuffer LightBuffer : register(b0)
{
    float3 lightDirection;
    float padding;
    row_major float4x4 cameraMatrix;
    row_major float4x4 projInvMatrix;
};
#else
cbuffer LightBuffer : register(b0)
{
    float3 lightDirection;
    float padding;
    float4x4 projMatrixInv;
    float4x4 viewMatrixInv;
    //float3 camPos;
    //float padding2;
};
#endif

cbuffer CamPosBuffer : register(b1)
{
    float3 camPos;
    float padding2;
};


float4 main(float4 position : SV_POSITION, float2 tex : TEXCOORD) : SV_TARGET
{
    float4 colors = colorTexture.Load(int3(position.xy, 0));
    
    float4 normals = normalTexture.Sample(SampleTypePoint, tex);
    float4 specular = specularTexture.Sample(SampleTypePoint, tex);
    
    float depthSample = (depthTexture.Sample(SampleTypePoint, tex).r * 2.0) - 1.0;
    float clipX = (tex.x * 2.0) - 1.0;
    float clipY = (tex.y * 2.0) - 1.0;
    clipY = -clipY;
    //clipX = -clipX;
    
    normals = (normals * 2.0) - 1.0;
    
#if 1
    float4 worldDepth = float4(clipX, clipY, depthSample, 1.0);
    float4 worldPosition = mul(worldDepth, projInvMatrix);
    worldPosition /= worldPosition.w;
    
    float4 worldSpacePos = mul(worldPosition, cameraMatrix);
    
    //float4 camSpaceDepth = mul(float4(camPos, 1), mvpMatrix);
    //camSpaceDepth /= camSpaceDepth.w;
    //float4 camSpacePos = mul(camSpaceDepth, viewInvMatrix);

    float3 camToFrag = worldSpacePos.xyz - camPos.xyz;
#else
    float4 viewDepth = float4(clipX, clipY, depthSample, 1.0);
    float4 viewPosition = mul(projMatrixInv, viewDepth);
    viewPosition /= viewPosition.w;

    float4 worldSpacePosition = mul(viewMatrixInv, viewPosition);
    // Calculate the amount of light on this pixel.
    float diffuseIntensity = saturate(dot(normalize(normals.xyz), normalize(-lightDirection.xyz)));
    
    float3 camToFrag = worldSpacePosition.xyz - camPos;
#endif
    float diffuseIntensity = saturate(dot(normalize(normals.xyz), normalize(-lightDirection.xyz)));

    float3 specularResult = Speculate(specular.xyz, 1, normalize(normals.xyz), normalize(-lightDirection), camToFrag, .5, 128);

    return colors * diffuseIntensity + float4(specularResult,1.0f);
}