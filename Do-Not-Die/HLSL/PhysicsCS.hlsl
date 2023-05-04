#define EPSILON 0.01f
#define PI 3.14159265359f

struct RayShape
{
    float3 start;
    float3 end;
};

struct TriangleShape
{
    uint including_node;
    float3 normal;
    float3 vertex0, vertex1, vertex2;
};

struct CapsuleShape
{
    float radius;
    float3 point_a;
    float3 point_b;
    int entity;
    int node_numbers[4];
};

struct SphereShape
{
    float radius;
    float3 center;
    int entity;
    int node_numbers[4];
};

struct CapsuleResult
{
    int entity;
    int collide_type;

    float3 floor_position;
    float4 wall_planes[4];
};

struct SphereResult
{
    int entity;
    int is_collide;
    float3 tri_normal;
};

struct CollisionResult
{
    CapsuleResult cap_result;

    SphereResult sphere_result;
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

struct SphereCallback
{
    int is_collide; // 0 : false, 1 : true
    float3 intersect_point;
    float3 tri_normal;
};

struct CapsuleCallback
{
    int collide_type; // 0 : none, 1 : floor, 2 : blocking
    float3 floor_position;
    float4 wall_plane;
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

float4 GetTriPlane(TriangleShape tri)
{
    float3 n = tri.normal;
    float d = n.x * tri.vertex0.x + n.y * tri.vertex0.y + n.z * tri.vertex0.z;    
    return float4(n, d);
}

float3 TriangleCenter(TriangleShape tri)
{
    return (tri.vertex0 + tri.vertex1 + tri.vertex2) / 2;
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

RayTriCallback RayToTriangle(RayShape ray, TriangleShape tri)
{
    RayTriCallback callback;
    callback.is_intersect = 0;
    callback.distance = 0;
    callback.intersect_point = float3(0, 0, 0);

    float3 n = tri.normal;
    float d = n.x * tri.vertex0.x + n.y * tri.vertex0.y + n.z * tri.vertex0.z;
    
    float4 plane = GetTriPlane(tri);
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

    return callback;
}

bool SphereToTriangle(float3 center, float radius, TriangleShape tri)
{    
    // step1 : check 3 points of tri    
    if (length(tri.vertex0 - center) <= radius)
        return true;

    if (length(tri.vertex1 - center) <= radius)
        return true;

    if (length(tri.vertex2 - center) <= radius)
        return true;

    // step2 : check sphere_to_tri ray
    RayShape sphere_to_tri;
    sphere_to_tri.start = center;
    sphere_to_tri.end = center + (-tri.normal * radius);
    RayTriCallback result = RayToTriangle(sphere_to_tri, tri);
    
    if (result.is_intersect) // step 3-1 : return intersect point
        return true;
    
    //else // step 3-2 : check edge_center segment
    //{
    //    float3 edge1_segment = PointLineSegment(center, tri.vertex0, tri.vertex1);
    //    if (length(edge1_segment - center) <= radius)
    //        return true;
        
    //    float3 edge2_segment = PointLineSegment(center, tri.vertex0, tri.vertex2);
    //    if (length(edge2_segment - center) <= radius)
    //        return true;
        
    //    float3 edge3_segment = PointLineSegment(center, tri.vertex2, tri.vertex1);
    //    if (length(edge3_segment - center) <= radius)
    //        return true;
    //}
    return false;
}

SphereCallback SphereToTriangle2(SphereShape sphere, TriangleShape tri)
{
    SphereCallback callback;
    callback.is_collide = false;
    callback.intersect_point = float3(0, 0, 0);
    callback.tri_normal = float3(0, 0, 0);

    // Compute the normal of the triangle plane
    float3 tri_vec1 = { tri.vertex1.x - tri.vertex0.x, tri.vertex1.y - tri.vertex0.y, tri.vertex1.z - tri.vertex0.z };
    float3 tri_vec2 = { tri.vertex2.x - tri.vertex0.x, tri.vertex2.y - tri.vertex0.y, tri.vertex2.z - tri.vertex0.z };
    float3 tri_normal = normalize(cross(tri_vec1, tri_vec2));

    // Compute the distance between the sphere center and the triangle plane
    double dist_plane = tri_normal.x * (sphere.center.x - tri.vertex0.x) + tri_normal.y * (sphere.center.y - tri.vertex0.y) + tri_normal.z * (sphere.center.z - tri.vertex0.z);
    dist_plane = dist_plane / length(tri_normal);

    // Check if the sphere intersects the plane containing the triangle
    if (dist_plane > sphere.radius) {
        return callback;
    }

    // Compute the barycentric coordinates of the sphere projection onto the triangle plane
    float3 sphere_proj =
    { sphere.center.x - dist_plane * tri_normal.x, sphere.center.y - dist_plane * tri_normal.y, sphere.center.z - dist_plane * tri_normal.z };
    double area1 = 0.5 * length(cross(tri_vec1, sphere_proj - tri.vertex0));
    double area2 = 0.5 * length(cross(tri_vec2, sphere_proj - tri.vertex0));
    double area3 = 0.5 * length(cross(tri_vec2, tri.vertex0 - tri.vertex1));
    double total_area = area1 + area2 + area3;
    double u = area1 / total_area;
    double v = area2 / total_area;
    double w = 1.0 - u - v;

    // Check if the sphere projection is inside the triangle
    if (u >= 0.0 && u <= 1.0 && v >= 0.0 && v <= 1.0 && w >= 0.0 && w <= 1.0) {
        // Compute the intersection points
        double dist_intersect = sqrt(sphere.radius * sphere.radius - dist_plane * dist_plane);
        float3 intersect1 =
        { sphere_proj.x + dist_intersect * tri_normal.x, sphere_proj.y + dist_intersect * tri_normal.y, sphere_proj.z + dist_intersect * tri_normal.z };
        float3 intersect2 =
        { sphere_proj.x - dist_intersect * tri_normal.x, sphere_proj.y - dist_intersect * tri_normal.y, sphere_proj.z - dist_intersect * tri_normal.z };


        float3 intersection_point1 = float3(intersect1.x * u, intersect1.y * v, intersect1.z * w);
        float3 intersection_point2 = float3(intersect2.x * u, intersect2.y * v, intersect2.z * w);

        callback.is_collide = true;
        callback.tri_normal = tri_normal;
        callback.intersect_point = (intersection_point1 + intersection_point2) / 2.0f;
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
    callback.wall_plane - float4(0, 0, 0, 0);
    
    // floor
    RayShape foot_ray;
    foot_ray.start = tip;
    foot_ray.end = base;
    
    RayTriCallback foot_callback = RayToTriangle(foot_ray, tri);
    if (foot_callback.is_intersect && DegreeBetweenVectors(GetRayVector(foot_ray), -tri.normal) <= 75)
    {// floor        
        callback.collide_type = 1;
        callback.floor_position = foot_callback.intersect_point;        
    }
    else
    { // wall
        float3 sphere_center = cap.point_b;
        float side = dot(tri.normal, sphere_center - tri.vertex0);
        if (side < 0.0f)
            tri.normal *= -1.0f;
            
        if (SphereToTriangle(sphere_center, cap.radius, tri))
        {
            callback.collide_type = 2;
            float4 tri_plane = GetTriPlane(tri);            
            callback.wall_plane = tri_plane;
        }
    }
    
    return callback;
}

StructuredBuffer<TriangleShape> level_triangles : register(t0);
StructuredBuffer<CapsuleShape> character_capsules : register(t1); // capsule_pool_32
StructuredBuffer<SphereShape> spheres : register(t2); // sphere_pool_32
RWStructuredBuffer<CollisionResult> collision_result : register(u0); // capsule_pool_32

[numthreads(32, 1, 1)]
void CS(
uint3 Gid : SV_GroupID,
uint3 DTid : SV_DispatchThreadID,
uint3 GTid : SV_GroupThreadID,
uint GI : SV_GroupIndex)
{
    uint triangle_count = 0;
    uint stride = 0;

    level_triangles.GetDimensions(triangle_count, stride);

    // Capsule Collision Check
    CapsuleShape capsule = character_capsules[GTid.x];

    if (capsule.radius > 0.999f)
    {
        CapsuleResult capsule_result;

        capsule_result.entity = capsule.entity;
        capsule_result.collide_type = 0;
        capsule_result.floor_position = GetCapsuleBase(capsule);

        capsule_result.wall_planes[0] = float4(0, 0, 0, 0);
        capsule_result.wall_planes[1] = float4(0, 0, 0, 0);
        capsule_result.wall_planes[2] = float4(0, 0, 0, 0);
        capsule_result.wall_planes[3] = float4(0, 0, 0, 0);

        bool floor_detected = false;
        bool wall_detected = false;

        for (int i = 0; i < triangle_count; ++i)
        {
            TriangleShape tri = level_triangles[i];

            if (capsule.node_numbers[0] != tri.including_node &&
                capsule.node_numbers[1] != tri.including_node &&
                capsule.node_numbers[2] != tri.including_node &&
                capsule.node_numbers[3] != tri.including_node)
                continue;

            CapsuleCallback callback = CapsuleToTriangle(capsule, tri);

            if (callback.collide_type == 1)
            {
                floor_detected = true;
                if (length(capsule.point_a - capsule_result.floor_position) > length(capsule.point_a - callback.floor_position))
                    capsule_result.floor_position = callback.floor_position;
            }

            if (callback.collide_type == 2) // wall
            {
                wall_detected = true;
                for (int j = 0; j < 4; ++j)
                {
                    if (length(capsule_result.wall_planes[j].xyz) < EPSILON)
                    {
                        capsule_result.wall_planes[j] = callback.wall_plane;
                        break;
                    }
                }
            }
        }

        if (floor_detected && wall_detected) // floor and walls
            capsule_result.collide_type = 3;
        else if (!floor_detected && wall_detected) // wall only
            capsule_result.collide_type = 2;
        else if (floor_detected && !wall_detected) // floor only
            capsule_result.collide_type = 1;
        else
            capsule_result.collide_type = 0; // none collision

        collision_result[GTid.x].cap_result = capsule_result;
    }

    // Sphere Triangle Check
    SphereShape sphere = spheres[GTid.x];

    if (sphere.radius > 0.999f)
    {
        SphereResult sphere_result;

        sphere_result.entity = sphere.entity;
        sphere_result.is_collide = 0;
        sphere_result.tri_normal = float3(0, 0, 0);

        for (int i = 0; i < triangle_count; ++i)
        {
            TriangleShape tri = level_triangles[i];

            if (sphere.node_numbers[0] != tri.including_node &&
                sphere.node_numbers[1] != tri.including_node &&
                sphere.node_numbers[2] != tri.including_node &&
                sphere.node_numbers[3] != tri.including_node)
                continue;

            SphereCallback sphere_callback = SphereToTriangle2(sphere, tri);

            if (SphereToTriangle(sphere.center, sphere.radius, tri))
            {
                sphere_result.is_collide = 1;

                float3 tri_vec1 = { tri.vertex1.x - tri.vertex0.x, tri.vertex1.y - tri.vertex0.y, tri.vertex1.z - tri.vertex0.z };
                float3 tri_vec2 = { tri.vertex2.x - tri.vertex0.x, tri.vertex2.y - tri.vertex0.y, tri.vertex2.z - tri.vertex0.z };
                sphere_result.tri_normal = normalize(cross(tri_vec1, tri_vec2));
           }
        }

        collision_result[GTid.x].sphere_result = sphere_result;
    }



}
