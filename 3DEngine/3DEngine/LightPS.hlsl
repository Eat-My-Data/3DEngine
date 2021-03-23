Texture2D colorTexture : register(t0);
Texture2D normalTexture : register(t1);

SamplerState SampleTypePoint : register(s0);

cbuffer LightBuffer
{
    float3 lightDirection;
    float padding;
};

struct VSOut
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};

float4 main(VSOut input) : SV_TARGET
{
    // Sample the colors from the color render texture
    float4 colors = colorTexture.Sample(SampleTypePoint, input.tex);

    // Sample the normals from the normal render texture
    float4 normals = normalTexture.Sample(SampleTypePoint, input.tex);

    // Invert the light direction for calculations.
    float3 lightDir = -lightDirection;

    // Calculate the amount of light on this pixel.
    float lightIntensity = saturate(dot(normals.xyz, lightDir));

    // Determine the final amount of diffuse color based on the color of the pixel combined with the light intensity.
    float4 outputColor = saturate(colors * lightIntensity);

    return outputColor;
}