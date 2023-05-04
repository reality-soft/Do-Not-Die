#include "include/VertexCommon.hlsli"

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
    
    float3 origin : NORMAL1;    
    float lod : TEXCOORD1;
    float3 view_dir : TEXCOORD2;
};

cbuffer CbShadowMatrix : register(b1)
{
    matrix shadow_view_proj;
};

VS_OUT VS(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;
    
    float4 local = float4(input.p, 1.0f);
    float4 project = mul(local, ViewProjection());

    output.p = project;
    output.n = normalize(input.n);
    output.t = input.t;
    
    output.lod = GetLod(input.p);
    output.view_dir = normalize(camera_world - local).xyz;
    output.origin = local;
    
    return output;
}