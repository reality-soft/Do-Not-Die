#include "stdafx.h"
#include "FX_Muzzle.h"
#include "SceneMgr.h"
#include "ResourceMgr.h"
#include "EffectMgr.h"
#include "Player.h"

using namespace reality;

void FX_Muzzle::OnInit(entt::registry& registry)
{
	FX_BaseEffectActor::OnInit(registry);

	AddEffectComponent("muzzle_effect", 0.5f);
	
	AddSpotLightComponent("SL_MuzzleFlame", 0.05f);
	auto& sl_comp = reg_scene_->get<C_SpotLight>(entity_id_);
	sl_comp.local = QUADTREE->camera_rotation_;

	RandomMarginFloat(sl_comp.spot_light.spot, 5.0f, false, false);
	RandomMarginFloat(sl_comp.spot_light.light_color.x, 0.4f, false, false);
	RandomMarginFloat(sl_comp.spot_light.light_color.y, 0.3f, false, false);
}

void FX_Muzzle::OnUpdate()
{
	Actor::OnUpdate();
}