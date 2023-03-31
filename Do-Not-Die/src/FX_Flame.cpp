#include "stdafx.h"
#include "FX_Flame.h"
#include "SceneMgr.h"
#include "ResourceMgr.h"

using namespace reality;

void FX_Flame::OnInit(entt::registry& registry)
{
	FX_BaseEffectActor::OnInit(registry);

	auto& effect_comp = registry.get<C_Effect>(GetEntityId());
	effect_comp.effect_id = "flame_effect";
	effect_comp.effect = *RESOURCE->UseResource<Effect>(effect_comp.effect_id);
}

void FX_Flame::OnUpdate()
{
	FX_BaseEffectActor::OnUpdate();
}
