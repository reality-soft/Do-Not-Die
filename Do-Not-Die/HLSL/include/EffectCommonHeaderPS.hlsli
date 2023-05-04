#define e 2.71828182846
#define EPSILON 0.001f

struct PointLight
{
	float4  light_color;
	float3  position;
	float   range;
	float3  attenuation;
	float   specular;
};

struct SpotLight
{
	float4  light_color;
	float3  position;
	float   range;
	float3  attenuation;
	float   specular;
	float3  direction;
	float   spot;
};

cbuffer CbGlobalLight : register(b0)
{
	float3 position;
	float brightness;

	float3 direction;
	float specular_strength;

	float4 ambient_up;
	float4 ambient_down;
	float4 ambient_range;
}

cbuffer CbPointLights : register(b2)
{
	PointLight point_lights[64];
}

cbuffer CbSpotLights : register(b3)
{
	SpotLight spot_lights[64];
}

cbuffer CbDistanceFog : register(b4)
{
	float4 fog_color;
	float3 eye_position;
	float fog_distance;
}

float4 ApplyDistanceFog(float4 color, float3 pixel_world)
{
	float3 fog_start = eye_position;
	float3 fog_end = normalize(pixel_world - fog_start) * fog_distance;

	float f = 1 / pow(e, pow(length(pixel_world - fog_start) / fog_distance, 2));
	return f * color + (1.0f - f) * fog_color;
}

Texture2D    g_txDiffuse			: register(t0);
Texture2D    g_txNormalMap			: register(t1);
Texture2D    g_txMetalic			: register(t2);
Texture2D    g_txRoughness			: register(t3);
Texture2D    g_txSepcular			: register(t4);
Texture2D    g_txAmbient			: register(t5);
Texture2D    g_txOpacity			: register(t6);

SamplerState g_SampleWrap		: register(s0);

struct PS_IN
{
	float4 p	: SV_POSITION;
	float4 c	: COLOR0;
	float2 t	: TEXCOORD0;
	float2 uv	: TEXCOORD1;
	float3 world_pos : TEXCOORD2;
};