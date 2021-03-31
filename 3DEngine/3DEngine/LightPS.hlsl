#include "ShaderOps.hlsl"

Texture2D colorTexture : register(t0);
Texture2D normalTexture : register(t1);
Texture2D specularTexture : register(t2);
Texture2D depthTexture : register(t3);

SamplerState SampleTypePoint : register(s0);

cbuffer LightBuffer : register(b0)
{
    float3 lightDirection;
    float padding;
    float4x4 mvpMatrix;
    //float3 camPos;
    //float padding2;
};

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
    float clipX = (tex.x - 0.5) * 2.0;
    float clipY = (tex.y - 0.5) * 2.0;
    clipY = -clipY;
   
    normals = (normals - .5) * 2;    
    
    float4 worldDepth = float4( clipX,clipY, depthSample, 1.0);
    float4 worldPosition = mul(worldDepth, mvpMatrix);
    worldPosition /= worldPosition.w;

    // Calculate the amount of light on this pixel.
    float diffuseIntensity = saturate(dot(normalize(normals.xyz), normalize(-lightDirection.xyz)));
    
    float3 camToFrag = worldPosition.xyz - camPos;
    
    float3 specularResult = Speculate(specular.xyz, 1, normals.xyz, -lightDirection, camToFrag, 1, 128);
    
    //return float4(specularResult, 1.0f);
    // Determine the final amount of diffuse color based on the color of the pixel combined with the light intensity.
    float4 outputColor = saturate(colors);

    return outputColor * diffuseIntensity + float4(specularResult,1.0f);
}