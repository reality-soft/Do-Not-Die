#include "stdafx.h"
#include "FX_Smash.h"

using namespace reality;

void FX_Smash::OnInit(entt::registry& registry)
{
	FX_BaseEffectActor::OnInit(registry);

	AddEffectComponent("smash_effect", 3.0f);
	
}

void FX_Smash::OnUpdate()
{
	Actor::OnUpdate();
}