#include "stdafx.h"
#include "FX_Explosion.h"
#include "SceneMgr.h"
#include "ResourceMgr.h"
#include "EffectMgr.h"
#include "GameEvents.h"

using namespace reality;

void FX_Explosion::OnInit(entt::registry& registry)
{
	FX_BaseEffectActor::OnInit(registry);

	AddEffectComponent("explosion_effect", 5.0f);

	AddPointLightComponent("PL_Explosion", 1.0f);

	AddSoundGeneratorComponent();

	EVENT->PushEvent<SoundGenerateEvent>(entity_id_, SFX, "S_Explosion_01.wav", 0.5f, false);
}

void FX_Explosion::OnUpdate()
{
	Actor::OnUpdate();

	auto& point_light_comp = reg_scene_->get<C_PointLight>(GetEntityId());
	float temp = 4.0f;
	point_light_comp.point_light.attenuation.x += point_light_comp.point_light.attenuation.x * TIMER->GetDeltaTime() * temp;
}
