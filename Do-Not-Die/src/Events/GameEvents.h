#pragma once
#include "stdafx.h"
#include "Engine_include.h"
#include "Shape.h"
#include "GameCharacter.h"
using namespace reality;

class AttackEvent : public Event
{
public:
	AttackEvent(RayShape ray, entt::entity actor_id) : ray_(ray), actor_id_(actor_id) {};

	virtual void Process() override {
		RayCallback raycallback = QUADTREE->Raycast(ray_);
		
		GameCharacter* character = SCENE_MGR->GetActor<GameCharacter>(actor_id_);

		float damage = character->GetCharacterDamage();

		if (raycallback.is_actor == true) {
			EVENT->PushEvent<TakeDamageEvent>(damage, raycallback.ent);
		}
	};
private:

	RayShape ray_;
	entt::entity actor_id_;
};

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

