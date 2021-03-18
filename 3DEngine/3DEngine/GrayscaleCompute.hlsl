
RWTexture2D<float4> TargetUAV : register(u0);

static const float3 groupColors[6] =
{
    float3(1, 0, 0),
    float3(0, 1, 0),
    float3(0, 0, 1),
    float3(1, 1, 0),
    float3(0, 1, 1),
    float3(1, 0, 1)
};

groupshared uint3 GroupAvgColor;

#define GRAYSCALE_COLOR 0
#define BRIGHTED 1
#define GROUP_COLOR 2
#define GROUP_AVG 3

#define SHADER_CONFIG GRAYSCALE_COLOR


[numthreads(32, 32, 1)]
void main( uint3 DTid : SV_DispatchThreadID, uint3 groupId : SV_GroupID, uint groupThreadIndex : SV_GroupIndex )
{    
    if (groupThreadIndex == 0)
    {
        GroupAvgColor = 0;
    }
    
    GroupMemoryBarrierWithGroupSync();
 
    uint2 texcoords = DTid.xy;
    float4 backBufferColor = (float4)0;
    
    if ( texcoords.x < 1280 && texcoords.y < 720)
    {
    
        backBufferColor = TargetUAV[texcoords]; // .Load(texcoords);
#if (SHADER_CONFIG == GRAYSCALE_COLOR)
        float avgColor = (backBufferColor.x + backBufferColor.y + backBufferColor.z) / 3.0f;
        TargetUAV[texcoords] = float4(avgColor.xxx, backBufferColor.a);
#elif (SHADER_CONFIG == BRIGHTED)
        backBufferColor.xyz *= 2.0f;
        TargetUAV[texcoords] = backBufferColor;
#elif (SHADER_CONFIG == GROUP_COLOR)
        backBufferColor.xyz *= groupColors[(groupId.x + groupId.y) % 6];
        TargetUAV[texcoords] = backBufferColor;
#elif (SHADER_CONFIG == GROUP_AVG)
        uint3 intColor = (uint3) (backBufferColor.xyz * 255.0f);
        
        InterlockedAdd(GroupAvgColor.x, intColor.x);
        InterlockedAdd(GroupAvgColor.y, intColor.y);
        InterlockedAdd(GroupAvgColor.z, intColor.z);
#endif //
    }
    GroupMemoryBarrierWithGroupSync();
        
#if (SHADER_CONFIG == GROUP_AVG)
    if (texcoords.x < 1280 && texcoords.y < 720)
    {
        float4 outColor = float4((GroupAvgColor.xyz / 1024) / 255.0f, backBufferColor.a);
        TargetUAV[texcoords] = outColor;
    }
#endif
}