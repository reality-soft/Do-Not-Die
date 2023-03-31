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
    float3 n : NORMAL;
    float2 d : TEXCOORD;
};

cbuffer CbShadowMatrix : register(b1)
{
    matrix shadow_view;
    matrix shadow_proj;
};

VS_OUT VS(VS_IN input)
{
    VS_OUT output = (VS_OUT)0;
    float4 local = float4(input.p, 1.0f);
    float4 world = mul(local, IdentityMatrix());
    float4 project = mul(world, mul(shadow_view, shadow_proj));
    
    output.p = project;
    output.d = output.p.zw;
    output.n = input.n;
    
    return output;
}