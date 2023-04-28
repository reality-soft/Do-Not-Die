#pragma once
#include "stdafx.h"
#include "Engine_include.h"
#include "Shape.h"
#include "Enemy.h"
#include "FX_BloodImpact.h"
#include "FX_ConcreteImpact.h"
#include "InGameScene.h"
using namespace reality;

class GameOverEvent : public Event
{
public:
	GameOverEvent() {};
	virtual void Process() override {
		bool game_over = true;
	}
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

class AttackEvent_SingleRay : public Event
{
public:
	AttackEvent_SingleRay(RayShape _ray, entt::entity actor_id) : ray(_ray), actor_id_(actor_id) {};

	virtual void Process() override 
	{
		GameCharacter* character = SCENE_MGR->GetActor<GameCharacter>(actor_id_);
		if (character == nullptr)
			return;

		XMVECTOR enemy_to_player = SCENE_MGR->GetPlayer<Character>(0)->GetCurPosition() - character->GetCurPosition();
		enemy_to_player = XMVector3Normalize(enemy_to_player);

		XMVECTOR enemy_attack_dir = _XMVECTOR3(ray.end) - _XMVECTOR3(ray.start);
		enemy_attack_dir = XMVector3Normalize(enemy_attack_dir);


		entt::entity actor_hit;
		float damage = character->GetCharacterDamage();

		if (character->tag == "enemy")
		{
			auto enemy_actor = SCENE_MGR->GetActor<Enemy>(actor_id_);
			enemy_actor->is_attacking_ = true;

			auto callback_car = QUADTREE->RaycastCarOnly(ray);
			if (callback_car.success)
			{
				*enemy_actor->targeting_car_health = max(0, *enemy_actor->targeting_car_health - 5);

				if (*enemy_actor->targeting_car_health == 0)
					EVENT->PushEvent<GameOverEvent>();
			}
			else
			{
				auto callback_actor = QUADTREE->RaycastActorOnly(ray);
				if (callback_actor.success)
				{
					if (SCENE_MGR->GetActor<GameCharacter>(callback_actor.ent)->tag == "player")
					{
						actor_hit = callback_actor.ent;
						EVENT->PushEvent<TakeDamageEvent>(damage, actor_hit);
						EFFECT_MGR->SpawnEffectFromNormal<FX_BloodImpact>(callback_actor.point, callback_actor.normal);
					}
				}
			}
			
		}
		else if (character->tag == "player")
		{
			auto callback_total = QUADTREE->Raycast(ray);
			if (callback_total.success)
			{
				if (callback_total.is_actor)
				{
					actor_hit = callback_total.ent;
					EVENT->PushEvent<TakeDamageEvent>(damage, actor_hit);
					EFFECT_MGR->SpawnEffectFromNormal<FX_BloodImpact>(callback_total.point, callback_total.normal);
				}
				else
				{
					EFFECT_MGR->SpawnEffectFromNormal<FX_ConcreteImpact>(callback_total.point, callback_total.normal);
				}
			}
		}
	};
private:
	RayShape ray;
	entt::entity actor_id_;
};

class MakeTextEvent : public Event
{
public:
	MakeTextEvent(string text) : text_(text) {};
	virtual void Process() override {
		auto ingamescene = (InGameScene*)SCENE_MGR->GetScene(INGAME).get();
		auto& ui_actor = ingamescene->GetUIActor();
		ui_actor.SetEventMsg(text_);
	};
private:
	string text_;
};

class GrenadeEvent : public Event
{
public:
	GrenadeEvent(XMVECTOR pos, float range, float damage) : pos_(pos), range_(range), damage_(damage) {};
	virtual void Process() override {
		auto ingamescene = (InGameScene*)SCENE_MGR->GetScene(INGAME).get();
		for (auto& pair : ingamescene->GetActors())
		{
			
			auto enemy = SCENE_MGR->GetActor<Enemy>(pair.first);

			if (enemy == nullptr)
				continue;

			float distance = XMVectorGetX(XMVector3Length(pos_ - enemy->GetCurPosition()));

			if (distance > range_)
				continue;

			float weight = distance / range_;

			enemy->TakeDamage(damage_ * weight);
		}
	};
private:
	XMVECTOR pos_;
	float range_;
	float damage_;
};

class KillEvent : public Event
{
public:
	KillEvent() {};
	virtual void Process() override {
		auto player = SCENE_MGR->GetPlayer<Player>(0);
		player->AddKillScore();
	};
private:
	XMVECTOR pos_;
	float range_;
	float damage_;
};
