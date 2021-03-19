#include "ShaderOps.hlsl"
#include "LightVectorData.hlsl"
#include "PointLight.hlsl"

cbuffer ObjectCBuf
{
    float specularIntensity;
    float specularPower;
    float padding[2];
};

Texture2D tex;
SamplerState splr;

struct PixelOutputType
{
    float4 color : SV_Target0;
    float4 normal : SV_Target1;
};

PixelOutputType main(float3 viewFragPos : Position, float3 viewNormal : Normal, float2 tc : Texcoord) : SV_Target
{
	//// renormalize interpolated normal
 //   viewNormal = normalize(viewNormal);
	//// fragment to light vector data
 //   const LightVectorData lv = CalculateLightVectorData(viewLightPos, viewFragPos);
	//// attenuation
 //   const float att = Attenuate(attConst, attLin, attQuad, lv.distToL);
	//// diffuse
 //   const float3 diffuse = Diffuse(diffuseColor, diffuseIntensity, att, lv.dirToL, viewNormal);
	//// specular
 //   const float3 specular = Speculate(diffuseColor, diffuseIntensity, viewNormal, lv.vToL, viewFragPos, att, specularPower);
	//// final color
 //   return float4(saturate((diffuse + ambient) * tex.Sample(splr, tc).rgb + specular), 1.0f);
	
    PixelOutputType output;


    // Sample the color from the texture and store it for output to the render target.
    output.color = tex.Sample(splr, tc);
	
    // Store the normal for output to the render target.
    output.normal = float4(viewNormal, 1.0f);

    return output;
}