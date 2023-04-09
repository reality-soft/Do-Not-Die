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
    int entity;
};

struct CollisionResult
{
    int entity;
    int collide_type;
    
    float3 floor_position;    
    RayShape blocking_rays[4];
};

struct RayTriCallback
{
    int is_intersect;
    float3 intersect_point;
    float distance;
};

struct CapsuleCallback
{
    int collide_type; // 0 : none, 1 : floor, 2 : blocking
    float3 floor_position;
    RayShape blocking_ray;
};

RayShape EmptyRay()
{
    RayShape empty_ray;
    empty_ray.start = float3(0, 0, 0);
    empty_ray.end = float3(0, 0, 0);
    
    return empty_ray;
}

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
    //float length_ = length(vec1) * length(vec2);
    //float angle = cos(dot_ / length_) * (180.0f / PI);
    //return angle;
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

float4 SphereEdgeIntersect(float3 v1, float3 v2, float3 center, float radius)
{    
    if (length(v1 - center) <= radius)
    {
        return float4(v1, true);
    }
    if (length(v2 - center) <= radius)
    {
        return float4(v2, true);
    }
    else
    {
        float3 direction = normalize(v2 - v1);
        float3 offset = center - v1;

        float3 t = dot(direction, offset) * direction;
        float d = length(t - direction);
        if (d < radius)
        {
            return float4(t - direction, true);
        }
    }
    
    return float4(0, 0, 0, false);
}

bool PointInTriangle(float3 p, TriangleShape tri)
{
    if (SameSide(p, tri.vertex0, tri.vertex1, tri.vertex2) &&
        SameSide(p, tri.vertex1, tri.vertex0, tri.vertex2) &&
        SameSide(p, tri.vertex2, tri.vertex0, tri.vertex1))
    {
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
        float t = (plane.w - dot(plane.xyz, ray.start.xyz)) / dot(plane.xyz, line_dir);
        intersect.xyz = ray.start.xyz + t * line_dir;
        intersect.w = true;
    }
    
    return intersect;
}

RayTriCallback RayToTriangle(RayShape ray, TriangleShape tri)
{
    RayTriCallback callback;
    callback.is_intersect = 0;
    callback.distance = 0;
    callback.intersect_point = float3(0 ,0, 0);
    
    //float3 e1 = tri.vertex1 - tri.vertex0;
    //float3 e2 = tri.vertex2 - tri.vertex0;
    //float3 n = normalize(cross(e1, e2));
    float3 n = tri.normal;
    float d = n.x * tri.vertex0.x + n.y * tri.vertex0.y + n.z * tri.vertex0.z;
    
    float4 plane = float4(n, d);
    float4 intersect_point = PlaneIntersectLine(plane, ray);
    
    if (intersect_point.w == true)
    {
        if (PointInTriangle(intersect_point.xyz, tri))
        {
            float d = length(intersect_point.xyz - ray.start);
            if (d <= length(GetRayVector(ray)))
            {
                callback.is_intersect = 1;
                callback.intersect_point = intersect_point.xyz;
                callback.distance = d;
            }
        }
    }
    
    //return float4(0, 0, 0, false);    
    return callback;    
}

CapsuleCallback CapsuleToTriangle(CapsuleShape cap, TriangleShape tri)
{ 
    float3 tip = GetCapsuleTip(cap);
    float3 base = GetCapsuleBase(cap);
    float3 capsule_dir = normalize(cap.point_a - cap.point_b);
    
    CapsuleCallback callback;
    callback.collide_type = 0;
    callback.floor_position = base;
    callback.blocking_ray = EmptyRay();
    
    // A ShpereToTri
    RayShape a_sphere_ray;
    a_sphere_ray.start = cap.point_a;
    a_sphere_ray.end = cap.point_a + (cap.radius * -tri.normal);
    RayTriCallback a_sphere_callback = RayToTriangle(a_sphere_ray, tri);
    
    if (a_sphere_callback.is_intersect == 1)
    {

        if (DegreeBetweenVectors(capsule_dir, -tri.normal) <= 80)
        {
            callback.collide_type = 1;
            //Set Standing Position
            float3 a_to_base = base - cap.point_a;
            float3 a_to_intersect = a_sphere_callback.intersect_point - cap.point_a;  
            
           
            //RayShape a_to_base_ray;
            //a_to_base_ray.start = cap.point_a;
            //a_to_base_ray.end = base;         
            //callback.floor_position = RayToTriangle(a_to_base_ray, tri).intersect_point;
            callback.floor_position = a_sphere_callback.intersect_point + (a_to_base - GetRayVector(a_sphere_ray));
        }
        else
        {
            callback.collide_type = 2;
            //Set Blocking Vector            
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
            
            callback.blocking_ray.start = float4(min_x, 0, min_z, 0);
            callback.blocking_ray.end = float4(max_x, 0, max_z, 0);
        }
    }
    else
    {
        //float4 a_to_edge1 = SphereEdgeIntersect(tri.vertex0, tri.vertex1, cap.point_a, cap.radius);
        //float4 a_to_edge2 = SphereEdgeIntersect(tri.vertex0, tri.vertex2, cap.point_a, cap.radius);
        //float4 a_to_edge3 = SphereEdgeIntersect(tri.vertex2, tri.vertex1, cap.point_a, cap.radius);
        
        //if (a_to_edge1.w == true)
        //{
        //    callback.collide_type = 1;
        //    callback.floor_position = a_to_edge1.xyz;
        //}
        //if (a_to_edge2.w == true)
        //{
        //    callback.collide_type = 1;
        //    callback.floor_position = a_to_edge2.xyz;
        //}
        //if (a_to_edge3.w == true)
        //{
        //    callback.collide_type = 1;
        //    callback.floor_position = a_to_edge3.xyz;
        //}
    }
    
    // B ShpereToTri
    RayShape b_sphere_ray;
    b_sphere_ray.start = cap.point_b;
    b_sphere_ray.end = cap.point_b + (cap.radius * -tri.normal);
    RayTriCallback b_sphere_callback = RayToTriangle(b_sphere_ray, tri);   

    if (b_sphere_callback.is_intersect == 1)
    {
        if (DegreeBetweenVectors(capsule_dir, -tri.normal) > 80)
        {
            callback.collide_type = 2;
            //Set Blocking Vector            
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
            
            callback.blocking_ray.start = float4(min_x, 0, min_z, 0);
            callback.blocking_ray.end = float4(max_x, 0, max_z, 0);
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
    result.entity = character_capsules[GTid.x].entity;
    result.collide_type = 0;
    result.floor_position = GetCapsuleBase(capsule);
    
    result.blocking_rays[0] = EmptyRay();
    result.blocking_rays[1] = EmptyRay();
    result.blocking_rays[2] = EmptyRay();
    result.blocking_rays[3] = EmptyRay();
    
    for (int i = 0; i < triangle_count; ++i)
    {
        TriangleShape tri = level_triangles[i];
        
        CapsuleCallback callback = CapsuleToTriangle(capsule, tri);
        if (callback.collide_type == 1) // floor
        {
            result.collide_type = 1;
            if (length(capsule.point_a - result.floor_position) > length(capsule.point_a - callback.floor_position))
                result.floor_position = callback.floor_position;
            
        }
        else if (callback.collide_type == 2) // wall
        {
            result.collide_type = 2;
            
            for (int j = 0; j < 4; ++j)
            {
                if (length(GetRayVector(result.blocking_rays[j])) < EPSILON)
                {
                    result.blocking_rays[j] = callback.blocking_ray;
                }
            }
        }
    }    
        
    collision_result[GTid.x] = result;
}
