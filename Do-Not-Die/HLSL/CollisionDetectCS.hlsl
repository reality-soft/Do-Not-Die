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

struct SphereTriCallback
{
    int is_intersect;
    float3 intersect_point;
    float penetration_depth;
    float3 sphere_base;
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

float3 PointLineSegment(float3 p, float3 start, float3 end)
{
    float3 segment = end - start;
    float3 diff = p - start;
    float t = saturate(dot(diff, segment) / dot(segment, segment));
    return start + t * segment;
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

SphereTriCallback SphereToTriangle(float3 center, float radius, TriangleShape tri)
{
    SphereTriCallback callback;
    
    callback.intersect_point = float3(0, 0, 0);
    callback.is_intersect = 0;
    callback.penetration_depth = 0;   
    float3 base = center + float3(0, -radius, 0);
    
    // step1 : check 3 points of tri    
    if (length(tri.vertex0 - center) <= radius)
    {
        callback.is_intersect = 1;
        callback.intersect_point = tri.vertex0;
        callback.penetration_depth = radius - length(tri.vertex0 - center);
        callback.sphere_base = base + (callback.intersect_point - (center + normalize(tri.vertex0 - center) * radius));
        return callback;
    }

    if (length(tri.vertex1 - center) <= radius)
    {
        callback.is_intersect = 1;
        callback.intersect_point = tri.vertex1;
        callback.penetration_depth = radius - length(tri.vertex1 - center);
        callback.sphere_base = base + (callback.intersect_point - (center + normalize(tri.vertex0 - center) * radius));
        return callback;
    }

    if (length(tri.vertex2 - center) <= radius)
    {
        callback.is_intersect = 1;
        callback.intersect_point = tri.vertex2;
        callback.penetration_depth = radius - length(tri.vertex2 - center);
        callback.sphere_base = base + (callback.intersect_point - (center + normalize(tri.vertex0 - center) * radius));
        return callback;
    }

    // step2 : check sphere_to_tri ray
    RayShape sphere_to_tri;
    sphere_to_tri.start = center;
    sphere_to_tri.end = center + (-tri.normal * radius);
    RayTriCallback result = RayToTriangle(sphere_to_tri, tri);
    
    if (result.is_intersect) // step 3-1 : return intersect point
    {
        callback.is_intersect = true;
        callback.intersect_point = result.intersect_point;
        callback.penetration_depth = radius - length(result.intersect_point - center);        
        callback.sphere_base = base + (callback.intersect_point - sphere_to_tri.end);
        return callback;
    }
    else // step 3-2 : check edge_center segment
    {
        float3 edge1_segment = PointLineSegment(center, tri.vertex0, tri.vertex1);
        if (length(edge1_segment - center) <= radius)
        {
            callback.is_intersect = true;
            callback.intersect_point = edge1_segment;
            callback.penetration_depth = radius - length(edge1_segment - center);
            callback.sphere_base = base + (edge1_segment - (center + normalize(edge1_segment - center) * radius));
            return callback;
        }
        
        float3 edge2_segment = PointLineSegment(center, tri.vertex0, tri.vertex2);
        if (length(edge2_segment - center) <= radius)
        {
            callback.is_intersect = true;
            callback.intersect_point = edge2_segment;
            callback.penetration_depth = radius - length(edge2_segment - center);
            callback.sphere_base = base + (edge2_segment - (center + normalize(edge2_segment - center) * radius));
            return callback;
        }        
        
        float3 edge3_segment = PointLineSegment(center, tri.vertex2, tri.vertex1);
        if (length(edge3_segment - center) <= radius)
        {
            callback.is_intersect = true;
            callback.intersect_point = edge3_segment;
            callback.penetration_depth = radius - length(edge3_segment - center);
            callback.sphere_base = base + (edge3_segment - (center + normalize(edge3_segment - center) * radius));
            return callback;
        }
    }    
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
    
    float3 n = tri.normal;
    float d = n.x * tri.vertex0.x + n.y * tri.vertex0.y + n.z * tri.vertex0.z;    
    float4 tri_plane = float4(n, d);
    
    RayShape capsule_line;
    capsule_line.start = tip;
    capsule_line.end = base;
    
    float4 trace_point = PlaneIntersectLine(tri_plane, capsule_line);
    
    if (trace_point.w == true)
    {
        float3 referent_point;
        
        if (PointInTriangle(trace_point.xyz, tri))
        {
            referent_point = PointLineSegment(trace_point.xyz, cap.point_b, cap.point_a);
        }
        else
        {
            float3 closest_tri_vertex = tri.vertex0;
        
            float length1 = length(trace_point.xyz - tri.vertex0);
            float length2 = length(trace_point.xyz - tri.vertex1);
            float length3 = length(trace_point.xyz - tri.vertex2);
        
            if (length1 < length2 && length1 < length3)
                closest_tri_vertex = tri.vertex0;
            else if (length2 < length1 && length2 < length3)
                closest_tri_vertex = tri.vertex1;
            else if (length3 < length1 && length3 < length2)
                closest_tri_vertex = tri.vertex2;
        
            referent_point = PointLineSegment(closest_tri_vertex, cap.point_b, cap.point_a);
        }        

        SphereTriCallback sphere_callback = SphereToTriangle(referent_point, cap.radius, tri);
        
        if (sphere_callback.is_intersect == 1)
        {
            callback.collide_type = 1;
            callback.floor_position = sphere_callback.sphere_base;
        }

    }
    else // convert capsule to aabb(wall)
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
