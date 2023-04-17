#include "include/VertexCommon.hlsli"

cbuffer cb_data : register(b1)
{
    matrix world_matrix;
    matrix local_matrix;
    matrix owner_local;
    matrix local_offset;
    matrix animation_matrix;
};

struct VS_IN
{
    float3 p : F3_POSITION;
    float3 n : F3_NORMAL;
    float2 t : F2_TEXTURE;
};
struct VS_OUT
{
    float4 p : SV_POSITION;
    float3 n : NORMAL0;
    float2 t : TEXCOORD0;
    
    float lod : TEXCOORD1;
    float3 view_dir : TEXCOORD2;
    float3 origin : NORMAL1;

    float4x4 normal_transform : TEXCOORD3;
};

VS_OUT VS(VS_IN input)
{
    VS_OUT output = (VS_OUT)0;

    float4 local = float4(input.p, 1.0f);
    local = mul(local, local_matrix);
    local = mul(local, local_offset);
    float4 animated_local = mul(local, animation_matrix);
    animated_local = mul(animated_local, owner_local);

    output.normal_transform = IdentityMatrix();

    float4 world = mul(animated_local, world_matrix);
    float4 projection = mul(world, ViewProjection());

    output.lod = GetLod(input.p);

    output.p = projection;
    output.n = input.n;
    output.t = input.t;

    output.lod = GetLod(input.p);
    output.view_dir = normalize(camera_world - world).xyz;
    output.origin = world;

    return output;
}