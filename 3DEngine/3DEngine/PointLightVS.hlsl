#include "Transform.hlsl"

struct VSOut
{
    float4 position : SV_POSITION;
    float2 tex : Texcoord;
};

VSOut main(float3 viewFragPos : Position)
{
    VSOut output;
    
    output.position = mul(float4(viewFragPos, 1.0f), modelViewProj);
    output.tex.x = (viewFragPos.x + .5) * .5;
    output.tex.y = (viewFragPos.y + .5) * .5;

    return output;
}