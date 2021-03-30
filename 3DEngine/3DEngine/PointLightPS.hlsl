Texture2D positionTexture : register(t0);
Texture2D colorTexture : register(t1);
Texture2D normalTexture : register(t2);
Texture2D specularTexture : register(t3);

SamplerState SampleTypePoint : register(s0);

cbuffer CBuf : register(b1)
{
    float4 color;
};


float4 main(float4 position : SV_POSITION,float2 tex : Texcoord) : SV_TARGET
{
    float2 screenPos;
    screenPos.x = position.x / 1280;
    screenPos.y = position.y / 720;
    
    // Sample the colors from the color render texture
    float4 colors = colorTexture.Sample(SampleTypePoint, screenPos);

    // Sample the normals from the normal render texture
    float4 normals = normalTexture.Sample(SampleTypePoint, screenPos);

    float4 specular = specularTexture.Sample(SampleTypePoint, screenPos);
    float4 positionSample = positionTexture.Sample(SampleTypePoint, screenPos);

    float4 dirToL = position - positionSample;
    // Invert the light direction for calculations.
    //float3 lightDir = -lightDirection;
    
    // Calculate the amount of light on this pixel.
    float lightIntensity = saturate(dot(normals.xyz, normalize(dirToL.xyz)));
    
    // Determine the final amount of diffuse color based on the color of the pixel combined with the light intensity.
    float4 outputColor = saturate(colors * normalize(lightIntensity) * specular.w);

    return colors;
}