#include "include/PixelCommon.hlsli"

struct PS_OUT
{    
    float4 p : SV_POSITION;
    float3 n : NORMAL0;
    float2 t : TEXCOORD0;
    float4 light_view_pos : TEXCOORD1;
};

Texture2D depth_map_tex: register(t0);
SamplerState depth_map_sampler : register(s0);

float4 PS(PS_OUT input) : SV_Target
{    
    //Shadow

    float2 projectTexCoord;
    float texture_depth_value;
    float light_depth_value;
    float bias = 0.01f;
    projectTexCoord.x = input.light_view_pos.x / input.light_view_pos.w / 2.0f + 0.5f;
    projectTexCoord.y = -input.light_view_pos.y / input.light_view_pos.w / 2.0f + 0.5f;

    float4 color = float4(0, 0, 0, 1);
    
    if ((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
    {
        // 투영 된 텍스처 좌표 위치에서 샘플러를 사용하여 깊이 텍스처에서 섀도우 맵 깊이 값을 샘플링합니다.
        texture_depth_value = depth_map_tex.Sample(depth_map_sampler, projectTexCoord).r;
        light_depth_value = input.light_view_pos.z / input.light_view_pos.w - bias;

        if (light_depth_value < texture_depth_value)
        {
            float lightIntensity = saturate(dot(input.n, -direction));
 
            if (lightIntensity > 0.0f)
            {
                color = float4(1.0f, 1.0f, 1.0f, 1.0f);
            }
        }
    }
    
    return color;
}