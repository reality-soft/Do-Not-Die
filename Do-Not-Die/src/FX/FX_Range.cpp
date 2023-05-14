#include "stdafx.h"
#include "FX_Range.h"

using namespace reality;

void FX_Range::OnInit(entt::registry& registry)
{
	FX_BaseEffectActor::OnInit(registry);

	AddEffectComponent("range_effect", 1.1f);
	
}

void FX_Range::OnUpdate()
{
	Actor::OnUpdate();
}