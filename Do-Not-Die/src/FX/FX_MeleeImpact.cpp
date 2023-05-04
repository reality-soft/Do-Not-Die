#include "stdafx.h"
#include "FX_MeleeImpact.h"
#include "SceneMgr.h"
#include "ResourceMgr.h"

using namespace reality;

void FX_MeleeImpact::OnInit(entt::registry& registry)
{
	FX_BaseEffectActor::OnInit(registry);

	AddEffectComponent("blood_effect_edited", 1.0f);

	AddSoundGeneratorComponent();

	//AddSoundQueue("S_WEP_Impact_Bullet_01.wav", SFX, false, 1.0f);

}