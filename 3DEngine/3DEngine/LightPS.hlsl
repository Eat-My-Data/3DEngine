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
    float4x4 mvpMatrix;
    //float3 camPos;
    //float padding2;
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
    // Sample the colors from the color render texture
    float4 colors = colorTexture.Load(int3(position.xy,0));
    
    // Sample the normals from the normal render texture
    float4 normals = normalTexture.Sample(SampleTypePoint, tex);
    float4 specular = specularTexture.Sample(SampleTypePoint, tex);
    
    float depthSample = depthTexture.Sample(SampleTypePoint, tex).r;
    float clipX = (tex.x * 2.0) - 1.0;
    float clipY = (tex.y * 2.0) - 1.0;
    clipY = -clipY;
    clipX = -clipX;
   
    normals = (normals * 2.0) - 1.0;    
#if 1
    float4 worldDepth = float4(clipX, clipY, 2.0 * depthSample - 1.0, 1.0);
    float4 worldPosition = mul(worldDepth, mvpMatrix);
    worldPosition /= worldPosition.w;

    // Calculate the amount of light on this pixel.
    float diffuseIntensity = saturate(dot(normalize(normals.xyz), normalize(-lightDirection.xyz)));
    
    float3 camToFrag = worldPosition.xyz - camPos;
#else
    float4 viewDepth = float4(clipX, clipY, depthSample, 1.0);
    float4 viewPosition = mul(projMatrixInv, viewDepth);
    viewPosition /= viewPosition.w;

    float4 worldSpacePosition = mul(viewMatrixInv, viewPosition);
    // Calculate the amount of light on this pixel.
    float diffuseIntensity = saturate(dot(normalize(normals.xyz), normalize(-lightDirection.xyz)));
    
    float3 camToFrag = worldSpacePosition.xyz - camPos;
#endif
    float3 specularResult = Speculate(specular.xyz, 1, normals.xyz, -lightDirection, camToFrag, .5, 128);
    
    //return float4(specularResult, 1.0f);
    // Determine the final amount of diffuse color based on the color of the pixel combined with the light intensity.
    float4 outputColor = saturate(colors);

    return colors * diffuseIntensity + float4(specularResult,1.0f);
}