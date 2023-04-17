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

cbuffer cb_skeletal_mesh : register(b1)
{
	matrix world;
	matrix local;
	matrix bind_pose[128];
	matrix prev_animation[128];
	matrix animation[128];
	matrix prev_slot_animation[128];
	matrix slot_animation[128];
	float4 weights[32];
	float  base_time_weight;
	float  slot_time_weight;
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

		float animation_weight = ((float[4])(weights[skeleton_index / 4]))[skeleton_index % 4];
		matrix animation_matrix_by_time = mul(prev_animation[skeleton_index], 1.0f - base_time_weight) + mul(animation[skeleton_index], base_time_weight);
		matrix slot_animation_matrix_by_time = mul(prev_slot_animation[skeleton_index], 1.0f - slot_time_weight) + mul(slot_animation[skeleton_index], slot_time_weight);
		matrix animation_matrix = mul(animation_matrix_by_time, 1.0f - animation_weight) + mul(slot_animation_matrix_by_time, animation_weight);
		matrix bind_pose_animation_matrix = mul(bind_pose[skeleton_index], animation_matrix);

		animation_vector += mul(local_vector, bind_pose_animation_matrix) * weight;
		anim_normal += mul(input.n, bind_pose_animation_matrix) * weight;
	}

	matrix world_matrix = mul(local, world);

	float4 world_vector = mul(animation_vector, world_matrix);
    float4 proj_vector = mul(world_vector, ViewProjection());

	output.lod = 0;
	output.p = proj_vector;
	output.n = mul(anim_normal, world);
	output.t = input.t;

	output.lod = GetLod(input.p);
	output.view_dir = normalize(camera_world - world_vector).xyz;
	output.origin = world_vector;

	return output;
}