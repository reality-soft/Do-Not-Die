struct TriangleShape
{
    uint index;
    float3 normal;
    float3 vertex0, vertex1, vertex2;
};

struct CapsuleShape
{
    float radius;
    float3 point_a;
    float3 point_b;
    min16uint triangles_to_cal[1000];
};

struct CollisionResult
{
    int result;
    float3 position;
};

StructuredBuffer<TriangleShape> level_triangles: register(t0);
StructuredBuffer<CapsuleShape> character_capsules : register(t1);
RWStructuredBuffer<CollisionResult> collision_result : register(u0);

[numthreads(64, 1, 1)]
void CS(
uint3 Gid  : SV_GroupID,
uint3 DTid : SV_DispatchThreadID, 
uint3 GTid : SV_GroupThreadID,
uint GI   : SV_GroupIndex)
{
    CollisionResult result;
    result.result = GTid.x;
    result.position = float3(1, 1, 1);
    collision_result[GTid.x] = result;
}