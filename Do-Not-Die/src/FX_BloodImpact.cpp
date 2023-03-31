#include "stdafx.h"
#include "FX_BloodImpact.h"
#include "SceneMgr.h"
#include "ResourceMgr.h"

using namespace reality;

void FX_BloodImpact::OnInit(entt::registry& registry)
{
	FX_BaseEffectActor::OnInit(registry);

	AddEffectComponent("blood_effect", 1.0f);

	AddSoundGeneratorComponent();

	AddSoundQueue("S_WEP_Fire_001.wav", SoundType::SFX, false, 10.0f);
}
