#include "include/PixelCommon.hlsli"

struct PS_IN
{
    float4 p : SV_POSITION;
    float3 n : NORMAL;
    float2 d : TEXCOORD;
};

float4 PS(PS_IN input) : SV_Target
{
    float shadow = input.d.x / input.d.y;
    return float4(shadow, shadow, shadow, 1.0f);
}