#include "include/EffectCommonHeaderPS.hlsli"


float4 PS(PS_IN input) : SV_Target
{
	float4 tex_color = g_txDiffuse.Sample(g_SampleWrap, input.t);
	float4 final_color = tex_color * input.c;
	final_color = ApplyDistanceFog(final_color, input.world_pos);
	final_color.a *= tex_color.a;
	return final_color;
}