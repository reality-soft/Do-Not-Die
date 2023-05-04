#include "stdafx.h"
#include "FX_BloodImpact.h"
#include "SceneMgr.h"
#include "ResourceMgr.h"
#include "GameEvents.h"

using namespace reality;

void FX_BloodImpact::OnInit(entt::registry& registry)
{
	FX_BaseEffectActor::OnInit(registry);

	AddEffectComponent("blood_effect_edited", 1.0f);
	AddSoundGeneratorComponent();

	EVENT->PushEvent<SoundGenerateEvent>(entity_id_, SFX, "S_WEP_Impact_Bullet_01.wav", false, 1.0f);
}
