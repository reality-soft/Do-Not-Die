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
}
