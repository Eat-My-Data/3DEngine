Texture2D colorTexture : register(t0);
Texture2D normalTexture : register(t1);
Texture2D specularTexture : register(t2);
Texture2D depthTexture : register(t3);

SamplerState SampleTypePoint : register(s0);

cbuffer CBuf : register(b1)
{
    float4 color;
    float4x4 mvpMatrix;
};

cbuffer CBuf : register(b2)
{
    float4 lightPosition;
};

float4 main(float4 position : SV_POSITION) : SV_TARGET
{
    float2 screenPos;
    screenPos.x = position.x / 1280;
    screenPos.y = position.y / 720;

    float4 colors = colorTexture.Sample(SampleTypePoint, screenPos);
    float4 normals = normalTexture.Sample(SampleTypePoint, screenPos);
    float4 specular = specularTexture.Sample(SampleTypePoint, screenPos);

    float depthSample = depthTexture.Sample(SampleTypePoint, screenPos).r;
    float clipX = (screenPos.x - 1.0) * 2.0;
    float clipY = (1 - screenPos.y) * 2.0;
    
    
    float4 worldDepth = float4(clipX, clipY, depthSample, 1.0);
    float4 worldPosition = mul(worldDepth, mvpMatrix);
    worldPosition /= worldPosition.w;
    
    float3 vToL = lightPosition.xyz - worldPosition.xyz;
    float len = length(vToL);
    float lightIntensity = saturate(dot(normalize(normals.xyz), normalize(vToL.xyz)));
    
    //float4 outputColor = saturate(color * colors * specular.w);
    //return sampledPosition;

    if (len > 30)
    {
        float4 outputColor = saturate( colors * specular.w);
        return outputColor * lightIntensity;
    }
    else
    {
        float4 outputColor = saturate(color * colors * specular.w);
        return outputColor * lightIntensity;
    }
}