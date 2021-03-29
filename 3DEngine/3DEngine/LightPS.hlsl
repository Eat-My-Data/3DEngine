Texture2D positionTexture : register(t0);
Texture2D colorTexture : register(t1);
Texture2D normalTexture : register(t2);
Texture2D specularTexture : register(t3);

SamplerState SampleTypePoint : register(s0);

cbuffer LightBuffer
{
    float3 lightDirection;
    float padding;
};


float4 main(float4 position : SV_POSITION, float2 tex : TEXCOORD) : SV_TARGET
{
    // Sample the colors from the color render texture
    float4 colors = colorTexture.Sample(SampleTypePoint, tex);

    // Sample the normals from the normal render texture
    float4 normals = normalTexture.Sample(SampleTypePoint, tex);

    float4 specular = specularTexture.Sample(SampleTypePoint, tex);
    
    // Invert the light direction for calculations.
    //float3 lightDir = -lightDirection;

    // Calculate the amount of light on this pixel.
    float lightIntensity = saturate(dot(normals.xyz, normalize(lightDirection)));
    
    // Determine the final amount of diffuse color based on the color of the pixel combined with the light intensity.
    float4 outputColor = saturate(colors * lightIntensity * specular.w);

    return outputColor;
}