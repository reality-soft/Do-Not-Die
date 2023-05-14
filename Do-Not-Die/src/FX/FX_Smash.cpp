#include "stdafx.h"
#include "FX_Smash.h"
#include "EventMgr.h"
#include "Events.h"
#include "Player.h"

using namespace reality;

void FX_Smash::OnInit(entt::registry& registry)
{
	FX_BaseEffectActor::OnInit(registry);

	AddEffectComponent("smash_effect", 2.0f);

	EVENT->PushEvent<CameraShakeEvent>(SCENE_MGR->GetPlayer<Player>(0)->GetEntityId(), 0.5f, 40.0f, 0.2f);
}

void FX_Smash::OnUpdate()
{
	Actor::OnUpdate();
}