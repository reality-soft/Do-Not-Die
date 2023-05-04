#pragma once
#include "stdafx.h"
#include "Engine_include.h"
#include "Shape.h"
#include "GeneralZombie.h"
#include "FX_BloodImpact.h"
#include "FX_ConcreteImpact.h"
#include "InGameScene.h"

using namespace reality;

class GameResultEvent : public Event
{
public:
	GameResultEvent(GameResultType result_type) : result(result_type) {};
	virtual void Process() override {
		auto ingame_scene = (InGameScene*)SCENE_MGR->GetScene(INGAME).get();
		if (ingame_scene)
			ingame_scene->game_result_type = result;
	}
	GameResultType result;
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
			
			auto enemy = SCENE_MGR->GetActor<GeneralZombie>(pair.first);

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

class SoundGenerateEvent : public Event
{
public:
	SoundGenerateEvent(entt::entity ent, SoundType sound_type, string sound_filename, float volume, bool is_looping)
		: ent_(ent), 
		sound_type_(sound_type),
		sound_filename_(sound_filename),
		volume_(volume),
		is_looping_(is_looping)
	{};

	virtual void Process() override{

		auto c_sound_gen = SCENE_MGR->GetScene(INGAME)->GetRegistryRef().try_get<C_SoundGenerator>(ent_);
		if (c_sound_gen == nullptr)
			return;

		SoundQueue sound_queue;
		sound_queue.sound_type = sound_type_;
		sound_queue.sound_filename = sound_filename_;
		sound_queue.sound_volume = volume_;
		sound_queue.is_looping = is_looping_;

		c_sound_gen->sound_queue_list.push(sound_queue);
	}

private:
	entt::entity ent_;
	SoundType sound_type_;
	string sound_filename_;
	float volume_;
	bool is_looping_;
};