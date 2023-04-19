#include "include/PixelCommon.hlsli"

struct PS_OUT
{
    float4 p : SV_POSITION;
    float3 n : NORMAL0;
    float2 t : TEXCOORD;
    
    float3 origin : NORMAL1;
    float4 projection : NORMAL2;
    
    float lod : TEXCOORD1;
    float3 view_dir : TEXCOORD2;
};

float4 Blur12x12(float2 t, Texture2D shadow_texture, SamplerState sample, float width)
{
    float4 color = float4(0.05, 0.05, 0.05, 0);
    
    float2 texcoord[12][12];
    float texel_size = 1.0f / width;
    
    float col = -6.f;
    for (int i = 0; i < 12; ++i)
    {
        float row = -6.f;
        for (int j = 0; j < 12; ++j)
        {
            texcoord[i][j] = t + float2(texel_size * row, texel_size * col);
            color += shadow_texture.Sample(sample, texcoord[i][j]) * 0.01f;
            row += 1.0f;
        }
        col += 1.0f;
    }
    
    return color;
}

Texture2D textures[7] : register(t0);
SamplerState samper_state : register(s0);

Texture2D shadow_map_tex : register(t8);
SamplerState shadow_sampler_clamp : register(s1);

float4 PS(PS_OUT input) : SV_Target
{
    if (length(input.origin - eye_position) > fog_distance * 2)
    {
        return fog_color;
    }
    
    float4 albedo = textures[0].Sample(samper_state, input.t);
    float4 roughness = textures[3].Sample(samper_state, input.t);
    
    float4 final_color = WhiteColor();

    albedo = ChangeSaturation(albedo, 1.3f);
    albedo = ChangeValue(albedo, 0.5f);
    albedo = ApplyHemisphericAmbient(input.n, albedo);
    
    final_color = ApplyCookTorrance(albedo, roughness.r, specular_strength, input.n, input.view_dir);
    final_color += ApplyPointLight(WhiteColor(), input.n, input.origin, input.view_dir);
    final_color += ApplySpotLight(WhiteColor(), input.n, input.origin, input.view_dir);
    
    //Shadow    
    //float2 projectTexCoord;
    //projectTexCoord.x = input.projection.x / input.projection.w / 2.0f + 0.5f;
    //projectTexCoord.y = -input.projection.y / input.projection.w / 2.0f + 0.5f;
    //
    //float4 shadow_color = Blur12x12(projectTexCoord, shadow_map_tex, shadow_sampler_clamp, 512);
    //final_color = albedo * shadow_color;
    
    //if (length(shadow_color.xyz) > 0.8f)
    //{
    //    final_color = ApplyCookTorrance(albedo, roughness.r, specular_strength, input.n, input.view_dir);
    //    final_color += ApplyPointLight(WhiteColor(), input.n, input.origin, input.view_dir);
    //    final_color += ApplySpotLight(WhiteColor(), input.n, input.origin, input.view_dir);
    //}
     
    final_color = ApplyDistanceFog(final_color, input.origin);

    return final_color;
}