Texture2D tex;
Texture2D spec;

SamplerState splr;

struct PSOut
{
    float4 position : SV_Target0;
    float4 color : SV_Target1;
    float4 normal : SV_Target2;
    float4 specular : SV_Target3;
};

PSOut main(float3 viewFragPos : Position, float3 viewNormal : Normal, float2 tc : Texcoord) : SV_Target
{
    PSOut output;

    // Sample the color from the texture and store it for output to the render target.
    output.position = float4(viewFragPos, 1.0f);
    output.color = tex.Sample(splr, tc);
    output.specular = spec.Sample(splr, tc);

	// 0 to 1 for normal
    viewNormal = normalize(viewNormal);
    viewNormal = (viewNormal + 1) / 2;
        
    // Store the normal for output to the render target.
    output.normal = float4(viewNormal, 1.0f);

    return output;
}