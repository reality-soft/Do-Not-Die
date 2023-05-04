#include "include/VertexCommon.hlsli"

cbuffer cb_data : register(b1)
{
    matrix world_matrix;
    matrix socket_matrix;
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

    output.normal_transform = mul(world_matrix, socket_matrix);
    
    float4 local = float4(input.p, 1.0f);
    float4 world = mul(local, output.normal_transform);
    float4 projection = mul(world, ViewProjection());

    output.lod = GetLod(input.p);

    output.p = projection;
    output.n = normalize(input.n);
    output.t = input.t;

    output.lod = GetLod(input.p);
    output.view_dir = normalize(camera_world - world).xyz;
    output.origin = world;

    output.normal_transform = IdentityMatrix();

    return output;
}