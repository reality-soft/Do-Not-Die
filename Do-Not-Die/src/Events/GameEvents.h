#pragma once
#include "stdafx.h"
#include "Engine_include.h"
#include "Shape.h"
#include "GameCharacter.h"
#include "FX_BloodImpact.h"
#include "FX_ConcreteImpact.h"
using namespace reality;

class TakeDamageEvent : public reality::Event
{
public:
	TakeDamageEvent(float damage, entt::entity actor_hit) : damage_(damage), actor_hit_(actor_hit) {}

	virtual void Process() override {
		GameCharacter* character = SCENE_MGR->GetActor<GameCharacter>(actor_hit_);
		character->TakeDamage(damage_);
	};
private:
	float damage_;
	entt::entity actor_hit_;
};

class AttackEvent : public Event
{
public:
	AttackEvent(vector<RayShape> rays, entt::entity actor_id) : rays_(rays), actor_id_(actor_id) {};

	virtual void Process() override {
		set<entt::entity> actors_hit;
		GameCharacter* character = SCENE_MGR->GetActor<GameCharacter>(actor_id_);
		float damage = character->GetCharacterDamage();

		for (const auto& cur_ray : rays_) {
			RayCallback raycallback = QUADTREE->Raycast(cur_ray);
			
			if (raycallback.is_actor) {
				actors_hit.insert(raycallback.ent);
			}

			if (raycallback.success && raycallback.is_actor)
				EFFECT_MGR->SpawnEffectFromNormal<FX_BloodImpact>(raycallback.point, raycallback.normal);

			else if (raycallback.success && !raycallback.is_actor)
				EFFECT_MGR->SpawnEffectFromNormal<FX_ConcreteImpact>(raycallback.point, raycallback.normal);
		}
		
		for (const auto& actor_hit : actors_hit) {
			EVENT->PushEvent<TakeDamageEvent>(damage, actor_hit);	
		}
	};
private:

	vector<RayShape> rays_;
	entt::entity actor_id_;
};

