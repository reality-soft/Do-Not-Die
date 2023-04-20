#include "WaveSystem.h"
#include "GameEvents.h"
#include "Player.h"
#include "Item.h"

void reality::WaveSystem::OnCreate(entt::registry& reg)
{
	item_spawns_ = QUADTREE->GetGuideLines("DND_ItemSpawn_1")->at(0);
	repair_spawns_ = QUADTREE->GetGuideLines("DND_RepairPart_1")->at(0);
	car_event_ = QUADTREE->GetGuideLines("DND_CarEvent_1")->at(0);

	CreateCarEventTriggers(_XMFLOAT3(car_event_.line_nodes.begin()->second), 200, 500);
}

void reality::WaveSystem::OnUpdate(entt::registry& reg)
{
	static int i = 1;
	if (i)
		RandomSpawnItem(15.0f); i = 0;

	float counting_time = world_env_->GetCountingTime();
	if (world_env_->IsDayChanged())
	{
		switch (world_env_->GetCurrentDay())
		{
		case Day::eNoon:
			countdown_timer_ = fabs(world_env_->GetTimeLimits().x * 2);
			RandomSpawnItem(30);
			break;
		case Day::eNight:
			countdown_timer_ = fabs(world_env_->GetTimeLimits().y * 2);
			wave_count_++;
			break;
		}
	}

	countdown_timer_ -= TM_DELTATIME;
	PlayerExtractRepair();
	PlayerRepairCar();

	if (car_repaired >= 5)
	{
		EVENT->PushEvent<GameOverEvent>();
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
	srand(time(NULL));

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
		SCENE_MGR->AddActor<Item>(item_type, _XMFLOAT3(spawn), trigger_radius);
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

void reality::WaveSystem::CreateExtractPoints(entt::registry& reg)
{
	srand(time(NULL));
	UINT repair_part_count = 5;
	set<UINT> random_indices;
	for (int i = 0; i < repair_part_count; ++i)
	{
		UINT index = rand() % repair_spawns_.line_nodes.size();

		if (random_indices.find(index) == random_indices.end())
			random_indices.insert(index);
		else
			i -= 1;
	} 

	for (const auto& node : repair_spawns_.line_nodes)
	{
		C_TriggerVolume trigger;
		trigger.tag = "extract";

		trigger.sphere_volume.center = _XMFLOAT3(node.second);
		trigger.sphere_volume.radius = 100;

		entt::entity trigger_entity = reg.create();
		reg.emplace<C_TriggerVolume>(trigger_entity, trigger);

		
		if (random_indices.find(node.first) != random_indices.end())
			repair_parts.insert(trigger_entity);
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
		player->InterectionRotate(_XMVECTOR3(trigger_comp->sphere_volume.center));
	}

	if (DINPUT->GetKeyState(DIK_E) == KEY_UP)
	{
		player->extract_during_time_ = 0.0f;
		player->controller_enable_ = true;
	}

	if (player->extract_during_time_ >= player->extract_time_takes_)
	{
		if (repair_parts.find(player->repair_extract_trigger) != repair_parts.end())
		{
			repair_parts.erase(player->repair_extract_trigger);

			auto trigger_comp = SCENE_MGR->GetScene(INGAME)->GetRegistryRef().try_get<C_TriggerVolume>(player->repair_extract_trigger);
			if (trigger_comp == nullptr)
				return;

			SpawnRepairItem(trigger_comp->sphere_volume.center);
		}
		else
		{
			// extract complete but there was no repair item

		}
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

	if (player->can_repair_car_ == false || player->HasRepairPart() == false)
		return;

	if (DINPUT->GetKeyState(DIK_R) == KEY_UP)
	{
		player->controller_enable_ = true;
		player->repair_during_time_ = 0.0f;
	}

	if (DINPUT->GetKeyState(DIK_R) == KEY_HOLD)
	{
		player->repair_during_time_ += TM_DELTATIME;
		player->InterectionRotate(car_event_.line_nodes.begin()->second);		
	}

	if (player->repair_during_time_ >= player->repair_time_takes_)
	{
		player->controller_enable_ = true;
		player->UseRepairPart();
		car_repaired += 1;
		player->repair_during_time_ = 0.0f;
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
