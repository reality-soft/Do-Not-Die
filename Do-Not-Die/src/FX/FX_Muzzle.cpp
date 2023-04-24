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
	
	//AddPointLightComponent("PL_Muzzle", 1.0f);
	AddSpotLightComponent("SL_MuzzleFlame", 0.05f);
	auto& sl_comp = reg_scene_->get<C_SpotLight>(entity_id_);
	sl_comp.local = QUADTREE->camera_rotation_;
	
}

void FX_Muzzle::OnUpdate()
{
	Actor::OnUpdate();
	
	//auto& point_light_comp = reg_scene_->get<C_PointLight>(GetEntityId());
	//float temp = 4.0f;
	//point_light_comp.point_light.attenuation.x += point_light_comp.point_light.attenuation.x * TIMER->GetDeltaTime() * temp;
}