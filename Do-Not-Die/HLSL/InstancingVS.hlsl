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
    float4 n : NORMAL;
    float2 t : TEXCOORD;  
    float lod : TEXCOORD1;
};

struct IntanceData
{
    matrix world_matrix;
};
  
StructuredBuffer<IntanceData> instance_buffer : register(t0);

VS_OUT VS(VS_IN input, uint inst : SV_InstanceID)
{
    VS_OUT output = (VS_OUT) 0;
    
    float4 local = float4(input.p, 1.0f);
    float4 normal = float4(input.n, 1.0f);
    
    float4 world = mul(local, instance_buffer[inst].world_matrix);
    float4 projection = mul(world, ViewProjection());
    
    output.lod = GetLod(input.p);
    output.p = projection;
    output.n = normal;
    output.t = input.t;

    return output;
}