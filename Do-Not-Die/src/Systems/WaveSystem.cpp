#include "WaveSystem.h"
#include "GameEvents.h"
#include "Player.h"
#include "GeneralZombie.h"
#include "Item.h"
#include "FX_Flame.h"
#include "FX_Smoke.h"

using namespace reality;

void reality::WaveSystem::OnCreate(entt::registry& reg)
{
	item_spawns_ = QUADTREE->GetGuideLines("DND_ItemSpawn_1")->at(0);
	repair_spawns_ = QUADTREE->GetGuideLines("DND_RepairPart_1")->at(0);
	car_event_ = QUADTREE->GetGuideLines("DND_CarEvent_1")->at(0);
	zomebie_tracks_ = QUADTREE->GetGuideLines("DND_NpcTrack_1");
	fx_car_fire_ = QUADTREE->GetGuideLines("DND_FX_CarFire_1")->at(0);
	fx_corpse_fire_ = QUADTREE->GetGuideLines("DND_FX_CorpseFire_1")->at(0);

	CreateExtractPoints(reg);
	CreateCarEventTriggers(_XMFLOAT3(car_event_.line_nodes.begin()->second), 150, 800);
	CreateStaticEffects();
}

void reality::WaveSystem::OnUpdate(entt::registry& reg)
{
	float counting_time = world_env_->GetCountingTime();
	if (world_env_->IsDayChanged())
	{
		switch (world_env_->GetCurrentDay())
		{
		case Day::eNoon:
			WaveFinish();
			countdown_timer_ = fabs(world_env_->GetTimeLimits().x * 2);
			break;
		case Day::eNight:
			WaveStart();
			countdown_timer_ = fabs(world_env_->GetTimeLimits().y * 2);
			break;
		}
	}
	zombie_count = SCENE_MGR->GetNumOfActor("enemy");
	countdown_timer_ -= TM_DELTATIME;
	PlayerExtractRepair();
	PlayerRepairCar();
	SpawnZombies(1.f, 1);
	SpawnCarSmokes();

	if (wave_count_ > 1 && SCENE_MGR->GetPlayer<Player>(0)->GetCurHp() > 0)
	{
		EVENT->PushEvent<GameResultEvent>(GameResultType::eGameCleared);
	}
	if (car_health <= 0)
	{
		EVENT->PushEvent<GameResultEvent>(GameResultType::eCarCrashed);
	}
}

void reality::WaveSystem::SetWorldEnv(Environment* env)
{
	world_env_ = shared_ptr<Environment>(env);
	countdown_timer_ = fabs(world_env_->GetTimeLimits().x * 2);
	for (const auto& node : item_spawns_.line_nodes)
	{
		item_table_.insert(make_pair(node.first, false));
	}
}

void reality::WaveSystem::RandomSpawnItem(float trigger_radius)
{
	list<UINT> empty_item_index;
	for (const auto& index : item_table_)
	{
		if (index.second == false)
			empty_item_index.push_back(index.first);
	}

	for (UINT index : empty_item_index)
	{
		ItemType item_type = (ItemType)(rand() % 7);

		const auto& spawn = item_spawns_.line_nodes[index];
		SCENE_MGR->GetScene(INGAME)->AddActor<Item>(item_type, _XMFLOAT3(spawn), trigger_radius);
		item_table_[index] = true;
	}
}

void reality::WaveSystem::CreateCarEventTriggers(XMFLOAT3 point, float repair_radius, float defense_radius)
{
	entt::entity reapair_volume_ent = SCENE_MGR->GetScene(INGAME).get()->GetRegistryRef().create();
	entt::entity defense_volume_ent = SCENE_MGR->GetScene(INGAME).get()->GetRegistryRef().create();

	C_TriggerVolume repair_trigger;
	repair_trigger.tag = "repair";
	repair_trigger.sphere_volume.center = point;
	repair_trigger.sphere_volume.radius = repair_radius;
	SCENE_MGR->GetScene(INGAME).get()->GetRegistryRef().emplace<C_TriggerVolume>(reapair_volume_ent, repair_trigger);
	
	C_TriggerVolume defense_trigger;
	defense_trigger.tag = "defense";
	defense_trigger.sphere_volume.center = point;
	defense_trigger.sphere_volume.radius = defense_radius;
	SCENE_MGR->GetScene(INGAME).get()->GetRegistryRef().emplace<C_TriggerVolume>(defense_volume_ent, defense_trigger);
}

void reality::WaveSystem::CreateStaticEffects()
{
	for (const auto& node : fx_corpse_fire_.line_nodes)
	{
		int random_scale = rand() % 5 + 5;
		entt::entity ent = EFFECT_MGR->SpawnEffect<reality::FX_Flame>(INGAME, node.second, XMQuaternionIdentity(), XMVectorReplicate(random_scale));
	}
}

void reality::WaveSystem::CreateExtractPoints(entt::registry& reg)
{
	for (const auto& node : repair_spawns_.line_nodes)
	{
		C_TriggerVolume trigger;
		trigger.tag = "extract";

		trigger.sphere_volume.center = _XMFLOAT3(node.second);
		trigger.sphere_volume.radius = 100;

		entt::entity trigger_entity = reg.create();
		reg.emplace<C_TriggerVolume>(trigger_entity, trigger);
	}
}

void reality::WaveSystem::PlayerExtractRepair()
{
	auto player = SCENE_MGR->GetPlayer<Player>(0);
	if (player == nullptr)
		return;

	if (player->can_extract_repair_ == false)
		return;

	auto trigger_comp = SCENE_MGR->GetScene(INGAME)->GetRegistryRef().try_get<C_TriggerVolume>(player->repair_extract_trigger);
	if (trigger_comp == nullptr)
		return;

	if (DINPUT->GetKeyState(DIK_E) == KEY_HOLD)
	{
		player->extract_during_time_ += TM_DELTATIME;
		player->InteractionRotate(_XMVECTOR3(trigger_comp->sphere_volume.center));
	}

	if (DINPUT->GetKeyState(DIK_E) == KEY_UP)
	{
		player->extract_during_time_ = 0.0f;
		player->controller_enable_ = true;
	}

	if (player->extract_during_time_ >= player->extract_time_takes_)
	{
		auto trigger_comp = SCENE_MGR->GetScene(INGAME)->GetRegistryRef().try_get<C_TriggerVolume>(player->repair_extract_trigger);
		if (trigger_comp == nullptr)
			return;

		SpawnRepairItem(trigger_comp->sphere_volume.center);

		EVENT->PushEvent<MakeTextEvent>("Extract Success!");

		DeleteExtractPoint(player->repair_extract_trigger);
		player->extract_during_time_ = 0.0f;
		player->can_extract_repair_ = false;
		player->controller_enable_ = true;
	}
}

void reality::WaveSystem::PlayerRepairCar()
{
	auto player = SCENE_MGR->GetPlayer<Player>(0);
	if (player == nullptr)
		return;

	if (player->can_repair_car_ == false)
		return;

	if(player->HasRepairPart() == false)
	{
		EVENT->PushEvent<MakeTextEvent>("There is no Repair Part in Inventory!");
		return;
	}
		

	if (DINPUT->GetKeyState(DIK_E) == KEY_UP)
	{
		player->controller_enable_ = true;
		player->repair_during_time_ = 0.0f;
	}

	if (DINPUT->GetKeyState(DIK_E) == KEY_HOLD)
	{
		player->repair_during_time_ += TM_DELTATIME;
		player->InteractionRotate(car_event_.line_nodes.begin()->second);		
	}

	if (player->repair_during_time_ >= player->repair_time_takes_)
	{
		player->controller_enable_ = true;
		player->UseRepairPart();
		car_health = min(100, car_health + 20);
		player->repair_during_time_ = 0.0f; 
		car_repair_count++;
		EVENT->PushEvent<MakeTextEvent>("Repair Success!");
	}
}

void reality::WaveSystem::SpawnRepairItem(XMFLOAT3 position)
{
	SCENE_MGR->AddActor<Item>(ItemType::eRepairPart, position, 30.0f);
}

void reality::WaveSystem::DeleteExtractPoint(entt::entity ent)
{
	auto player = SCENE_MGR->GetPlayer<Player>(0);
	if (player == nullptr)
		return;

	if (SCENE_MGR->GetScene(INGAME)->GetRegistryRef().valid(ent))
	{
		SCENE_MGR->GetScene(INGAME)->GetRegistryRef().destroy(ent);
	}
}

void reality::WaveSystem::SpawnZombies(float interval, UINT count)
{
	if (zombie_spawn_ == false)
		return;

	static float cur_time = 0.0f;

	cur_time += TM_DELTATIME;

	if (zombie_spawn_count_ >= count)
		return;
	

	if (cur_time < interval)
		return;

	auto enemy_entity = SCENE_MGR->AddActor<GeneralZombie>();
	// setting a character into quad tree
	if (QUADTREE->RegistDynamicCapsule(enemy_entity) == false)
	{
		EVENT->PushEvent<DeleteActorEvent>(enemy_entity);
	}
	else
	{
		auto enemy_actor = SCENE_MGR->GetActor<GeneralZombie>(enemy_entity);
		enemy_actor->targeting_car_health = &car_health;

		int guidline_index = rand() % zomebie_tracks_->size();
		int mesh_index = rand() % enemy_meshes.size();
		
		guidline_index = 4;

		vector<XMVECTOR> target_poses;
		for (const auto& target_pos : zomebie_tracks_->at(guidline_index).line_nodes) {
			target_poses.push_back(target_pos.second);
		}
		enemy_actor->SetBehaviorTree(target_poses);
		enemy_actor->SetMeshId(enemy_meshes[mesh_index]);

		cur_time = 0.0f;
		zombie_spawn_count_ += 1;
	}
	
}

void reality::WaveSystem::SpawnCarSmokes()
{
	for (int i = 0; i < 100; i += 20)
	{
		int index = i / 20;

		if (i >= car_health || car_health == 0)
		{
			if (spawned_car_fired_.find(index) != spawned_car_fired_.end())
				continue;

			auto node = fx_car_fire_.line_nodes.at(index);
			int random_scale = rand() % 5 + 5;
			entt::entity ent = EFFECT_MGR->SpawnEffect<reality::FX_Smoke>(INGAME, node, XMQuaternionIdentity(), XMVectorReplicate(random_scale));
			spawned_car_fired_.insert(make_pair(index, ent));
		}
		else
		{
			if (spawned_car_fired_.find(index) != spawned_car_fired_.end())
			{
				SCENE_MGR->DestroyActor(spawned_car_fired_.find(index)->second);
				spawned_car_fired_.erase(index);
			}
		}
	}
}

XMVECTOR reality::WaveSystem::GetCarPosition()
{
	return car_event_.line_nodes.begin()->second;
}


void reality::WaveSystem::WaveStart()
{
	zombie_spawn_ = true;
}

void reality::WaveSystem::WaveFinish()
{
	wave_count_++;
	zombie_spawn_count_ = 0;
	RandomSpawnItem(30);
}