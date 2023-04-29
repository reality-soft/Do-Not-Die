#include "FX_Smoke.h"

using namespace reality;

void FX_Smoke::OnInit(entt::registry& registry)
{
	FX_BaseEffectActor::OnInit(registry);

	AddEffectComponent("smoke_effect", -1.0f);
}
