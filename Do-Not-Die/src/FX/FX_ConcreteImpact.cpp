#include "stdafx.h"
#include "FX_ConcreteImpact.h"
#include "SceneMgr.h"
#include "ResourceMgr.h"
#include "GameEvents.h"

using namespace reality;

void FX_ConcreteImpact::OnInit(entt::registry& registry)
{
	FX_BaseEffectActor::OnInit(registry);

	AddEffectComponent("concrete_effect_edited", 1.0f);

	AddSoundGeneratorComponent();

	EVENT->PushEvent<SoundGenerateEvent>(entity_id_, SFX, "S_WEP_Impact_Bullet_03.wav", 1.0f, false);
}

