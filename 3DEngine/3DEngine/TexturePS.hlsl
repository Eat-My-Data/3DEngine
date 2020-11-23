Texture2D tex;

SamplerState splr;

float4 main( float2 tx : TexCoord) : SV_TARGET
{
	return tex.Sample( splr,tx );
}