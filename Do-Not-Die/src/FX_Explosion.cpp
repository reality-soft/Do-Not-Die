#include "stdafx.h"
#include "FX_Explosion.h"
#include "SceneMgr.h"
#include "ResourceMgr.h"
#include "EffectMgr.h"

using namespace reality;

void FX_Explosion::OnInit(entt::registry& registry)
{
	FX_BaseEffectActor::OnInit(registry);

	AddEffectComponent("explosion_effect", 5.0f);

	AddPointLightComponent("PL_Explosion", 1.0f);

	AddSoundGeneratorComponent();

	AddSoundQueue("S_Explosion_01.wav", SFX, false, 30.0f);
}
