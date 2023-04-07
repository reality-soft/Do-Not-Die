#define EPSILON 0.01f
#define PI 3.14159265359f

struct RayShape
{
    float3 start;
    float3 end;
};

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
};

struct CollisionResult
{
    bool is_collide;
    float3 floor_position;
    float3 blocking_vectors[4];
};

struct CapsuleCallback
{
    int collide_type; // 0 : none, 1 : floor, 2 : blocking
    float3 floor_position;
    float3 blocking_vector;
};

float3 GetCapsuleTip(CapsuleShape capsule)
{
    float3 tip = capsule.point_b;
    tip.y += capsule.radius;
    
    return tip;
}

float3 GetCapsuleBase(CapsuleShape capsule)
{
    float3 base = capsule.point_a;
    base.y -= capsule.radius;
    
    return base;
}

float3 GetRayVector(RayShape ray)
{
    return ray.end - ray.start;
}

float DegreeBetweenVectors(float3 vec1, float3 vec2)
{
    float dot_ = dot(normalize(vec1), normalize(vec2));
    return acos(dot_) * (180.0f / PI);
}

bool SameSide(float3 p1, float3 p2, float3 a, float3 b)
{
    float3 cp1 = cross(b - a, p1 - a);
    float3 cp2 = cross(b - a, p2 - a);
    
    if (dot(cp1, cp2) >= 0)
        return true;

    return false;
}

bool ParallelVector(float3 vec1, float3 vec2)
{
    float dot_ = dot(normalize(vec1), normalize(vec2));
    bool is_parallel = abs(dot_ - 1.0f) < EPSILON || abs(dot_ + 1.0f) < EPSILON;
    return is_parallel;
}

bool PointInTriangle(float3 p, TriangleShape tri)
{
    if (SameSide(p, tri.vertex0, tri.vertex1, tri.vertex2) &&
        SameSide(p, tri.vertex1, tri.vertex0, tri.vertex2) &&
        SameSide(p, tri.vertex2, tri.vertex0, tri.vertex1))
    {
        float3 cross_ = cross(tri.vertex0 - p, tri.vertex1 - p);
        if (ParallelVector(cross_, tri.normal))
            return true;
    }
    
    return false;
}

float4 PlaneIntersectLine(float4 plane, RayShape ray)
{
    float4 intersect;
    float3 line_dir = normalize(GetRayVector(ray));

    if (abs(dot(plane.xyz, line_dir)) < EPSILON)
    {
        intersect.xyz = ray.end;
        intersect.w = false;
    }
    else
    {            
        float t = (plane.w - dot(plane.xyz, ray.start)) / dot(plane.xyz, line_dir);
        intersect.xyz = ray.start + t * line_dir;
        intersect.w = true;
    }
    
    return intersect;
}

float4 RayToTriangle(RayShape ray, TriangleShape tri)
{
    float3 e1 = tri.vertex1 - tri.vertex0;
    float3 e2 = tri.vertex2 - tri.vertex0;
    float3 n = normalize(cross(e1, e2));
    float d = n.x * tri.vertex0.x + n.y * tri.vertex0.y + n.z * tri.vertex0.z;
    
    float4 plane = float4(n, d);
    float4 intersect_point = PlaneIntersectLine(plane, ray);
    
    if (intersect_point.w == true)
    {
        if (PointInTriangle(intersect_point.xyz, tri))
        {            
            float distance = length(intersect_point.xyz - ray.start);
            float ray_length = length(GetRayVector(ray));
        
            if (distance <= ray_length)
            {
                return float4(intersect_point.xyz, true);
            }
        }
    }
    
    return float4(intersect_point.xyz, false);    
}

CapsuleCallback CapsuleToTriangle(CapsuleShape cap, TriangleShape tri)
{    
    float3 tip = GetCapsuleTip(cap);
    float3 base = GetCapsuleBase(cap);
    
    CapsuleCallback callback;
    callback.collide_type = 0;
    callback.floor_position = base;
    callback.blocking_vector = float3(0, 0, 0);
    
    RayShape a_tri_ray;
    RayShape b_tri_ray;
    
    a_tri_ray.start = cap.point_a;
    b_tri_ray.start = cap.point_b;
    
    a_tri_ray.end = a_tri_ray.start + (-tri.normal * cap.radius);
    b_tri_ray.end = b_tri_ray.start + (-tri.normal * cap.radius);
    
    float4 a_tri_intersect = RayToTriangle(a_tri_ray, tri);
    float4 b_tri_intersect = RayToTriangle(b_tri_ray, tri);
    
    if (a_tri_intersect.w == true || b_tri_intersect.w == true)
    {
        float3 capsule_dir = normalize(base - tip);
        
        float angle = DegreeBetweenVectors(capsule_dir, -tri.normal);
        if (angle <= 80.0f)
        {
            callback.collide_type = 1; // floor
            callback.floor_position = a_tri_intersect;
        }
        else
        {
            callback.collide_type = 2; // wall
            
            float min_x = tri.vertex0.x;
            min_x = min(min_x, tri.vertex1.x);
            min_x = min(min_x, tri.vertex2.x);
            
            float min_z = tri.vertex0.z;
            min_z = min(min_z, tri.vertex1.z);
            min_z = min(min_z, tri.vertex2.z);
            
            float max_x = tri.vertex0.x;
            max_x = max(max_x, tri.vertex1.x);
            max_x = max(max_x, tri.vertex2.x);
            
            float max_z = tri.vertex0.z;
            max_z = max(max_z, tri.vertex1.z);
            max_z = max(max_z, tri.vertex2.z);            
            
            callback.blocking_vector = normalize(float3(max_x, 0, max_z) - float3(min_x, 0, min_z));
        }
    }    

    return callback;
}

StructuredBuffer<TriangleShape> level_triangles: register(t0);
StructuredBuffer<CapsuleShape> character_capsules : register(t1); // capsule_pool_64
RWStructuredBuffer<CollisionResult> collision_result : register(u0); // capsule_pool_64

[numthreads(64, 1, 1)]
void CS(
uint3 Gid  : SV_GroupID,
uint3 DTid : SV_DispatchThreadID, 
uint3 GTid : SV_GroupThreadID,
uint GI   : SV_GroupIndex)
{
    CapsuleShape capsule = character_capsules[GTid.x];
    if (capsule.radius < 1.0f)
        return;
    
    uint triangle_count = 0;
    uint stride = 0;
    
    level_triangles.GetDimensions(triangle_count, stride);
    
    CollisionResult result;
    result.is_collide = false;
    result.floor_position = GetCapsuleBase(capsule);
    result.blocking_vectors[0] = float3(0, 0, 0);
    result.blocking_vectors[1] = float3(0, 0, 0);
    result.blocking_vectors[2] = float3(0, 0, 0);
    result.blocking_vectors[3] = float3(0, 0, 0);
    
    int blocking_vector_index = 0;
    //triangle_count = 11102;
    for (int i = 0; i < triangle_count; ++i)
    {
        TriangleShape tri = level_triangles[i];
        
        CapsuleCallback callback = CapsuleToTriangle(capsule, tri);
        if (callback.collide_type == 1) // floor
        {
            result.is_collide = true;
            if (length(capsule.point_a - result.floor_position) > length(capsule.point_a - callback.floor_position))
                result.floor_position = callback.floor_position;
            
        }
        else if (callback.collide_type == 2 && blocking_vector_index < 4) // wall
        {
            result.is_collide = true;
            
            for (int j = 0; j < 4; ++j)
            {
                if (length(result.blocking_vectors[j]) < EPSILON)
                {
                    result.blocking_vectors[j] = callback.blocking_vector;
                }
            }
        }
    }
        
    collision_result[GTid.x] = result;
}
