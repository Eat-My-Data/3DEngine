cbuffer ComputeParticlesConstants : register(b0)
{
    float3 centerOfGravity;
    float gravityStrength;
    float deltaTime;
	uint numTotalParticles;
    float pad[2];
};

struct Body
{
    float3 pos;
    float3 vel;
    float3 acc;
    float mass;
};

RWStructuredBuffer<Body> particles : register(u0);

float3 Apply_Forces(Body body)
{
    float3 grav_acc = normalize(centerOfGravity - body.pos) * gravityStrength; // 9.81m/s^2 down in the Z-axis
    float3 drag_force = 0.5 * 0.1 * (body.vel * abs(body.vel)); // D = 0.5 * (rho * C * Area * vel^2)
    float3 drag_acc = drag_force / body.mass; // a = F/m
    return grav_acc - drag_acc;
}

[numthreads(1024, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    if (DTid.x < numTotalParticles)
    {
        Body body = particles[DTid.x];
        float3 new_pos = body.pos + body.vel * deltaTime + body.acc * (deltaTime * deltaTime * 0.5);
        float3 new_acc = Apply_Forces(body); // only needed if acceleration is not constant
        float3 new_vel = body.vel + (body.acc + new_acc) * (deltaTime * 0.5);
        body.pos = new_pos;
        body.vel = new_vel;
        body.acc = new_acc;
        particles[DTid.x].pos = body.pos;
        particles[DTid.x].vel = body.vel;
        particles[DTid.x].acc = body.acc;
    }
}