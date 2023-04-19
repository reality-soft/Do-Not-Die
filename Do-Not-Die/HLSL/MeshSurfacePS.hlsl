#include "include/PixelCommon.hlsli"

struct PS_OUT
{
    float4 p : SV_POSITION;
    float3 n : NORMAL0;
    float2 t : TEXCOORD;

    float lod : TEXCOORD1;
    float3 view_dir : TEXCOORD2;
    float3 origin : NORMAL1;

    float4x4 normal_transform : TEXCOORD3;
};

Texture2D textures[7] : register(t0);
SamplerState samper_state : register(s0);

float4 PS(PS_OUT input) : SV_Target
{    
    float4 albedo = textures[0].Sample(samper_state, input.t);
    float4 final_color = WhiteColor();
    float4 roughness = textures[3].Sample(samper_state, input.t);

    float4 normal = textures[1].Sample(samper_state, input.t) * 2.0f - 1.0f;
    if (length(saturate(normal)) <= EPSILON) {
        normal = float4(input.n, 0.0f);
    }

    normal = normalize(normal);
    
    normal = mul(normal, input.normal_transform);

    albedo = ChangeSaturation(albedo, 1.3f);
    albedo = ChangeValue(albedo, 0.5f);
    albedo = ApplyHemisphericAmbient(normal.xyz, albedo);
    
    final_color = ApplyCookTorrance(albedo, 0.2f, specular_strength, normal.xyz, input.view_dir);
    final_color += ApplyPointLight(WhiteColor(), normal.xyz, input.origin, input.view_dir);
    final_color += ApplySpotLight(WhiteColor(), normal.xyz, input.origin, input.view_dir);
    final_color = ApplyDistanceFog(final_color, input.origin);
    
    return final_color;
}