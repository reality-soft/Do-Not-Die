
#define EPSILON 0.001f
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


float2x3 GetTipBase(CapsuleShape capsule)
{
    float3 tip = capsule.point_b;
    float3 base = capsule.point_a;
    tip.y += capsule.radius;
    base.y -= capsule.radius;
    return float2x3(tip, base);
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

    return abs(dot_ - 1.0f) < EPSILON || abs(dot_ + 1.0f) < EPSILON;
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

float3 PlaneIntersectLine(float4 plane, RayShape ray)
{
    float3 line_dir = normalize(GetRayVector(ray));
    float denom = dot(line_dir, plane.xyz);

    if (abs(denom) < EPSILON)
        return ray.start;
    
    float3 plane_pos = -plane.w / plane.xyz;
    
    float t = dot(plane_pos - ray.start, plane.xyz) / denom;
    return ray.start + t * line_dir;
}

float4 RayToTriangle(RayShape ray, TriangleShape tri)
{
    float4 plane = { tri.normal.x, tri.normal.y, tri.normal.z, -dot(tri.normal, tri.vertex0) };

    float3 intersect_point = PlaneIntersectLine(plane, ray);
    if (PointInTriangle(intersect_point, tri))
    {
        float distance = length(intersect_point - ray.start);
        float ray_length = length(GetRayVector(ray));
        
        if (distance <= ray_length)
        {
            float4(intersect_point, true);
        }
    }
    
    return float4(intersect_point, false);;
}

CapsuleCallback CapsuleToTriangle(CapsuleShape cap, TriangleShape tri)
{    
    float3 tip = GetTipBase(cap)[0];
    float3 base = GetTipBase(cap)[1];
    
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
            
            if (length(cap.point_a - callback.floor_position) > length(cap.point_a - a_tri_intersect.xyz))
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
    result.floor_position = GetTipBase(capsule)[1];
    result.blocking_vectors[0] = float3(0, 0, 0);
    result.blocking_vectors[1] = float3(0, 0, 0);
    result.blocking_vectors[2] = float3(0, 0, 0);
    result.blocking_vectors[3] = float3(0, 0, 0);
    
    int blocking_vector_index = 0;
    
    for (int i = 0; i < triangle_count; ++i)
    {
        TriangleShape tri = level_triangles[i];
        
        CapsuleCallback callback = CapsuleToTriangle(capsule, tri);
        if (callback.collide_type == 0)
            break;
        else if (callback.collide_type == 1) // floor
        {
            result.is_collide = true;
            result.floor_position = callback.floor_position;
        }
        else if (callback.collide_type == 2) // wall
        {
            result.blocking_vectors[blocking_vector_index++] = callback.blocking_vector;
        }
    }
        
    collision_result[GTid.x] = result;
}
