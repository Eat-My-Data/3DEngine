cbuffer LightCBuf
{
	float3 lightPos;
	float3 ambient;
	float3 diffuseColor;
	float diffuseIntensity;
	float attConst;
	float attLin;
	float attQuad;
};

cbuffer ObjectCBuf
{
	float specularIntensity;
	float specularPower;
	bool normalMapEnabled;
	float padding[1];
};

cbuffer TransformCBuf
{
    matrix modelView;
    matrix modelViewProj;
};

Texture2D tex;
Texture2D nmap : register(t2);

SamplerState splr;


float4 main(float3 viewPos : Position, float3 viewNormal : Normal, float2 tc : Texcoord) : SV_Target
{
	// sample normal from map if normal mapping enabled
    if (normalMapEnabled)
    {
		// sample and unpack normal data
        const float3 normalSample = nmap.Sample(splr, tc).xyz;
        float3 tanNormal;
        tanNormal.x = normalSample.x * 2.0f - 1.0f;
        tanNormal.y = -normalSample.y * 2.0f + 1.0f;
        tanNormal.z = -normalSample.z * 2.0f + 1.0f;
		// bring normal from object space into view space
        viewNormal = normalize(mul(tanNormal, (float3x3) modelView));
    }
	// fragment to light vector data
	const float3 vToL = lightPos - viewPos;
	const float distToL = length( vToL );
	const float3 dirToL = vToL / distToL;
	// attenuation
	const float att = 1.0f / (attConst + attLin * distToL + attQuad * (distToL * distToL));
	// diffuse intensity
    const float3 diffuse = diffuseColor * diffuseIntensity * att * max(0.0f, dot(dirToL, viewNormal));
	// reflected light vector
    const float3 w = viewNormal * dot(vToL, viewNormal);
	const float3 r = w * 2.0f - vToL;
	// calculate specular intensity based on angle between viewing vector and reflection vector, narrow with power function
	const float3 specular = att * (diffuseColor * diffuseIntensity) * specularIntensity * pow( max( 0.0f, dot( normalize( -r ), normalize( viewPos ) ) ), specularPower );
	// final color
	return float4( saturate( (diffuse + ambient) * tex.Sample( splr, tc ).rgb + specular ), 1.0f );
} 