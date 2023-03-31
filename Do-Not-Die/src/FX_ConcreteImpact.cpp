#include "stdafx.h"
#include "FX_ConcreteImpact.h"
#include "SceneMgr.h"
#include "ResourceMgr.h"

using namespace reality;

void FX_ConcreteImpact::OnInit(entt::registry& registry)
{
	FX_BaseEffectActor::OnInit(registry);

	AddEffectComponent("concrete_effect", 1.0f);

	AddSoundGeneratorComponent();

	AddSoundQueue("S_WEP_Fire_001.wav", SoundType::SFX, false, 10.0f);
}

