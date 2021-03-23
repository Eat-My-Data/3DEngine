#include "Transform.hlsl"

struct VSOut
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};

VSOut main(float4 position : POSITION, float2 tex : TEXCOORD0)
{
    VSOut output;
    
    // Change the position vector to be 4 units for proper matrix calculations.
    position.w = 1.0f;

    // Calculate the position of the vertex against the view, and projection matrices.
    output.position = mul(position, modelView);
    output.position = mul(output.position, modelViewProj);
    
    // Store the texture coordinates for the pixel shader.
    output.tex = tex;
    
    return output;
}