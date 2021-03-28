#include "Transform.hlsl"

struct VSOut
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD;
};

VSOut main(uint vertId : SV_VertexID)
{
    VSOut output;
    
    // Change the position vector to be 4 units for proper matrix calculations.
    //position.w = 1.0f;

    // Calculate the position of the vertex against the view, and projection matrices.
    //output.position = mul(position, modelView);
    //output.position = mul(output.position, modelViewProj);
    
    float x = -1.0 + float((vertId & 1) << 2);
    float y = -1.0 + float((vertId & 2) << 1);
    output.tex.x = (x + 1.0) * 0.5;
    output.tex.y = (1.0 - y) * 0.5;

    output.position = float4(x,y,0,1);
    //output.position = mul(output.position, modelView);
    //output.position = mul(output.position, modelViewProj);
    
    return output;
}