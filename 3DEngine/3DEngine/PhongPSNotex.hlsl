#include "ShaderOps.hlsl"
#include "LightVectorData.hlsl"
#include "PointLight.hlsl"

cbuffer ObjectCBuf
{
    float4 materialColor;
    float4 specularColor;
    float specularPower;
};

struct PSOut
{
    float4 color : SV_Target0;
    float4 normal : SV_Target1;
};

PSOut main(float3 viewFragPos : Position, float3 viewNormal : Normal) : SV_Target
{
    PSOut output;
    
    output.color = materialColor;
    	
    // Store the normal for output to the render target.
    output.normal = float4(viewNormal, 1.0f);

    return output;
}