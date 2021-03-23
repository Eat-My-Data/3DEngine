cbuffer ParticlesCB : register(b0)
{
    row_major float4x4 MVPMatrix;
}

struct Particle
{
    float3 position;
    float3 velocity;
    float3 acceleration;

    float mass;
};

StructuredBuffer<Particle> Particles : register(t0);

float4 main( uint vertId : SV_VertexID ) : SV_POSITION
{
    return mul(float4(Particles[vertId].position,1.0f), MVPMatrix);
}