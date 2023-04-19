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
    float2 d : NORMAL1;    
};

cbuffer CbShadowMatrix : register(b1)
{
    matrix shadow_view_proj;
};

VS_OUT VS(VS_IN input)
{
    VS_OUT output = (VS_OUT)0;
    float4 local = float4(input.p, 1.0f);
    float4 light_project = mul(local, shadow_view_proj);
    
    output.p = light_project;
    output.d = output.p.zw;
    output.n = input.n;
 
    return output;
}