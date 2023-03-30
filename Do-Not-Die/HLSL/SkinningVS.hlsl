#include "include/VertexCommon.hlsli"

struct VS_IN
{
	float3 p : F3_POSITION;
	float3 n : F3_NORMAL;
	float4 c : F4_COLOR;
	float2 t : F2_TEXTURE;
	float4 i : F4_INDEX;
	float4 w : F4_WEIGHT;
};
struct VS_OUT
{
	float4 p : SV_POSITION;
	float4 n : NORMAL;
	float2 t : TEXCOORD0;

	float lod : TEXCOORD1;
	float3 view_dir : TEXCOORD2;
	float3 origin : NORMAL1;
};

cbuffer cb_transform_data : register(b1)
{
	matrix world;
};

cbuffer cb_bone_data : register(b2)
{
	matrix bind_pose[128];
	matrix animation[128];
}

VS_OUT VS(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;
	float4 local_vector = float4(input.p, 1.0f);

	float4 animation_vector = 0;
	float4 anim_normal = 0;

	for (int s = 0; s < 4; s++)
	{
		uint skeleton_index = input.i[s];
		float weight = input.w[s];

		matrix animation_matrix = mul(bind_pose[skeleton_index], animation[skeleton_index]);

		animation_vector += mul(local_vector, animation_matrix) * weight;
		anim_normal += mul(input.n, animation_matrix) * weight;
	}

	float4 world_vector = mul(animation_vector, world);
	float4 view_vector = mul(world_vector, view_matrix);
	float4 proj_vector = mul(view_vector, projection_matrix);

	output.lod = 0;
	output.p = proj_vector;
	output.n = mul(anim_normal, world);
	output.t = input.t;

	output.lod = GetLod(input.p);
	output.view_dir = normalize(camera_world - world_vector).xyz;
	output.origin = world_vector;

	return output;
}