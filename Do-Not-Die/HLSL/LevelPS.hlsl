#include "include/PixelCommon.hlsli"

struct PS_OUT
{
    float4 p : SV_POSITION;
    float3 n : NORMAL0;
    float2 t : TEXCOORD;
    
    float3 origin : NORMAL1;    
    float lod : TEXCOORD1;
    float3 view_dir : TEXCOORD2;
    
    //float4 light_view : TEXCOORD3;
};

Texture2D textures[7] : register(t0);
SamplerState samper_state : register(s0);

//Texture2D depth_map_tex : register(t8);
//SamplerState depth_sampler_clamp : register(s1);

//Texture2D cubemap_depth_1[6] : register(t9);
//Texture2D cubemap_depth_2[6] : register(t15);
//Texture2D cubemap_depth_3[6] : register(t21);
//Texture2D cubemap_depth_4[6] : register(t27);
//Texture2D cubemap_depth_5[6] : register(t33);
//Texture2D cubemap_depth_6[6] : register(t36);
//Texture2D cubemap_depth_7[6] : register(t42);
//Texture2D cubemap_depth_8[6] : register(t48);
//Texture2D cubemap_depth_9[6] : register(t54);

float4 PS(PS_OUT input) : SV_Target
{
    if (length(input.origin - eye_position) > fog_distance * 2)
    {
        return fog_color;
    }
    
    float4 albedo = textures[0].Sample(samper_state, input.t);
    float4 roughness = textures[3].Sample(samper_state, input.t);
    
    float4 final_color = WhiteColor();
    
    albedo = ChangeSaturation(albedo, 1.4f);
    albedo = ChangeValue(albedo, 0.8f);
    albedo = ApplyHemisphericAmbient(input.n, albedo);
    
   // Shadow
    //float2 projectTexCoord;
    //projectTexCoord.x = input.light_view.x / input.light_view.w / 2.0f + 0.5f;
    //projectTexCoord.y = -input.light_view.y / input.light_view.w / 2.0f + 0.5f;
    
    //float shadow_value = depth_map_tex.Sample(depth_sampler_clamp, projectTexCoord).r;   
    //float depth_value = input.light_view.z / input.light_view.w - 0.01f;
    
    //if (depth_value < shadow_value)
    //{
    //    final_color = ApplyCookTorrance(albedo, roughness.r, specular_strength, input.n, input.view_dir);
    //}
    //else
    //{
    //    final_color = albedo * 0.2f;
    //}
    
    final_color = ApplyCookTorrance(albedo, roughness.r, specular_strength, input.n, input.view_dir);
    final_color += ApplyPointLight(WhiteColor(), input.n, input.origin, input.view_dir);
    final_color += ApplySpotLight(WhiteColor(), input.n, input.origin, input.view_dir);    
    final_color = ApplyDistanceFog(final_color, input.origin);

    return final_color;
}