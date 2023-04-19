#include "WaveSystem.h"
#include "Player.h"
#include "Item.h"

void reality::WaveSystem::OnCreate(entt::registry& reg)
{
	QUADTREE->ImportGuideLines("ItemSpawn.mapdat", GuideType::eSpawnPoint);
	QUADTREE->ImportGuideLines("DND_RepairPart_1.mapdat", GuideType::eSpawnPoint);

	item_spawns_ = QUADTREE->GetGuideLines("ItemSpawn")->at(0);
	repair_spawns_ = QUADTREE->GetGuideLines("DND_RepairPart_1")->at(0);
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
			RandomSpawnItem(15.0f);
			break;
		case Day::eNight:
			countdown_timer_ = fabs(world_env_->GetTimeLimits().y * 2);
			wave_count_++;
			break;
		}
	}

	countdown_timer_ -= TM_DELTATIME;
	PlayerExtractRepair();
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

	if (player->can_extract_repair == false)
		return;

	static float extract_time = 0.0f;

	if (DINPUT->GetKeyState(DIK_E) == KEY_HOLD)
		extract_time += TM_DELTATIME;

	if (DINPUT->GetKeyState(DIK_E) == KEY_UP)
		extract_time = 0.0f;

	if (extract_time >= player->extract_time_takes)
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
		extract_time = 0.0f;
		player->can_extract_repair = false;
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
