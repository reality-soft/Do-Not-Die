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
    float4 roughness = textures[3].Sample(samper_state, input.t);
    float4 specular = textures[4].Sample(samper_state, input.t);
    
    float4 final_color = WhiteColor();
    
    float3 normal = mul(input.n, input.normal_transform);
    normal = normalize(normal);

    albedo = ChangeSaturation(albedo, 1.4f);
    albedo = ChangeValue(albedo, 0.8f);
    albedo = ApplyHemisphericAmbient(normal.xyz, albedo);
    
    final_color = ApplyCookTorrance(albedo, roughness.r, specular.r, normal.xyz, input.view_dir);
    final_color += ApplyPointLight(WhiteColor(), normal.xyz, input.origin, input.view_dir);
    final_color += ApplySpotLight(WhiteColor(), normal.xyz, input.origin, input.view_dir);
    final_color = ApplyDistanceFog(final_color, input.origin);
    
    return final_color;
}