#include "stdafx.h"
#include "FX_Flame.h"
#include "SceneMgr.h"
#include "ResourceMgr.h"
#include "EffectMgr.h"

using namespace reality;

void FX_Flame::OnInit(entt::registry& registry)
{
	FX_BaseEffectActor::OnInit(registry);

	AddEffectComponent("flame_effect_edited", -1.0f);

	AddPointLightComponent("PL_Flame", -1.0f);

	//auto& pointlight_comp = registry.get<C_PointLight>(GetEntityId());
	//pointlight_comp.light_color = { 1.0f, 0.35f, 0.0f, 1.0f };
	//pointlight_comp.lifetime = -1.0f;
	//pointlight_comp.timer = 0.0f;
	//pointlight_comp.attenuation = { 1.0f, 0.0f, 0.0f };
	//pointlight_comp.attenuation_level = { 0.0f, 0.05f, 0.0f };
	//pointlight_comp.range = 500.0f;

	//EFFECT_MGR->SpawnEffect<FX_Flame>(XMLoadFloat3(&pointlight_comp.position), XMQuaternionIdentity(), XMVectorSet(10.0f, 10.0f, 10.0f, 0.0f));
}
