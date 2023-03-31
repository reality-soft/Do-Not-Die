#include "include/PixelCommon.hlsli"

struct PS_OUT
{
    float4 p : SV_POSITION;
    float3 n : NORMAL0;
    float2 t : TEXCOORD;
    
    float lod : TEXCOORD1;
    float3 view_dir : TEXCOORD2;
    float3 origin : NORMAL1;
    float4 shadow_tex : TEXCOORD3;
};

Texture2D textures[7] : register(t0);
SamplerState samper_state : register(s0);

Texture2D shadow_map : register(t8);
SamplerState shadow_sampler : register(s1);

float4 PS(PS_OUT input) : SV_Target
{
    float4 albedo = textures[0].Sample(samper_state, input.t);
    float4 roughness = textures[3].Sample(samper_state, input.t);
    
    float4 final_color = WhiteColor();
    
    albedo = ChangeSaturation(albedo, 1.3f);
    float4 middle_albedo = albedo;
    albedo = ChangeValue(albedo, 0.5f);
    albedo = ApplyHemisphericAmbient(input.n, albedo);
    
    final_color = ApplyCookTorrance(albedo, roughness.r, specular_strength, input.n, input.view_dir);
    final_color += ApplyPointLight(middle_albedo, input.n, input.origin, input.view_dir);
    final_color += ApplySpotLight(middle_albedo, input.n, input.origin, input.view_dir);
            
    // Shadow
    float2 shadow_tex_coord;
    shadow_tex_coord.x = input.shadow_tex.x / input.shadow_tex.w / 2.0f + 0.5f;
    shadow_tex_coord.y = input.shadow_tex.y / input.shadow_tex.w / 2.0f + 0.5f;
    
    if ((saturate(shadow_tex_coord.x) == shadow_tex_coord.x) && (saturate(shadow_tex_coord.y) == shadow_tex_coord.y))
    {
        float depth_value = shadow_map.Sample(shadow_sampler, shadow_tex_coord).r;
        float light_depth = input.shadow_tex.z / input.shadow_tex.w + 0.1f;
        
        if (light_depth > depth_value)
        {
            final_color = final_color * float4(1.5f, 0.0f, 0.0f, 1.0f);
            final_color.w = 1.0f;
        }
    }
    
    final_color = ApplyDistanceFog(final_color, input.origin);
    return final_color;
}