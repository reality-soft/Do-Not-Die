#pragma once
#include "stdafx.h"
#include "Engine_include.h"
#include "Shape.h"
#include "NormalZombie.h"
#include "FX_BloodImpact.h"
#include "FX_MeleeImpact.h"
#include "FX_ConcreteImpact.h"
#include "InGameScene.h"

using namespace reality;

class GameResultEvent : public Event
{
public:
	GameResultEvent(GameResultType result_type) : result(result_type) {};
	virtual void Process() override {
		auto ingame_scene = (InGameScene*)SCENE_MGR->GetScene(INGAME).get();
		if (ingame_scene && ingame_scene->game_result_type == GameResultType::eNone)
			ingame_scene->game_result_type = result;

		
		if (result != GameResultType::eGameCleared)
		{
			FMOD_MGR->Stop("S_Night_BGM.wav");
			FMOD_MGR->Stop("S_Day_BGM.wav");
			FMOD_MGR->Play("S_GameOver_BGM.mp3", MUSIC, true, 1.0f, {});
		}
			

	}
	
private:
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
	MakeTextEvent(string text, float x) : text_(text), x_(x) {};
	virtual void Process() override {
		auto ingamescene = (InGameScene*)SCENE_MGR->GetScene(INGAME).get();
		auto& ui_actor = ingamescene->GetUIActor();
		ui_actor.SetEventMsg(text_, x_);
	};
private:
	string text_;
	float x_;
};

class GrenadeEvent : public Event
{
public:
	GrenadeEvent(XMVECTOR pos, float range, float damage) : pos_(pos), range_(range), damage_(damage) {};
	virtual void Process() override {
		auto ingamescene = (InGameScene*)SCENE_MGR->GetScene(INGAME).get();
		for (auto& pair : ingamescene->GetActors())
		{
			
			auto enemy = SCENE_MGR->GetActor<NormalZombie>(pair.first);

			if (enemy == nullptr)
				continue;

			float distance = XMVectorGetX(XMVector3Length(pos_ - enemy->GetCurPosition()));

			if (distance > range_)
				continue;

			float weight = 1.0f - distance / range_;

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

class WalkEvent : public Event
{
public:
	WalkEvent() {};
	virtual void Process() override {
		static int count = 0;
		auto player = SCENE_MGR->GetPlayer<Player>(0);
    EVENT->PushEvent<SoundGenerateEvent>(player->entity_id_, SFX, "S_CH_Footstep_001.wav", 1.0f, false);
	};
};

