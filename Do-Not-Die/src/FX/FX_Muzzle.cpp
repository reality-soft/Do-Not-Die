#include "stdafx.h"
#include "FX_Muzzle.h"
#include "SceneMgr.h"
#include "ResourceMgr.h"
#include "EffectMgr.h"

using namespace reality;

void FX_Muzzle::OnInit(entt::registry& registry)
{
	FX_BaseEffectActor::OnInit(registry);

	AddEffectComponent("muzzle_effect", 1.0f);

	AddPointLightComponent("PL_Muzzle", 1.0f);

}

void FX_Muzzle::OnUpdate()
{
	Actor::OnUpdate();

	auto& point_light_comp = reg_scene_->get<C_PointLight>(GetEntityId());
	float temp = 4.0f;
	point_light_comp.point_light.attenuation.x += point_light_comp.point_light.attenuation.x * TIMER->GetDeltaTime() * temp;
}