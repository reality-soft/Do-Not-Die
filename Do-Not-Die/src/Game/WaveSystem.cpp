#include "WaveSystem.h"
#include "Item.h"

void reality::WaveSystem::OnCreate(entt::registry& reg)
{
	QUADTREE->ImportGuideLines("ItemSpawn.mapdat", GuideType::eSpawnPoint);
	item_spawns_ = QUADTREE->GetGuideLines("ItemSpawn");
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
}

void reality::WaveSystem::SetWorldEnv(Environment* env)
{
	world_env_ = shared_ptr<Environment>(env);
	countdown_timer_ = fabs(world_env_->GetTimeLimits().x * 2);
	for (const auto& node : item_spawns_->at(0).line_nodes)
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

		const auto& spawn = item_spawns_->at(0).line_nodes[index];
		SCENE_MGR->AddActor<Item>(item_type, _XMFLOAT3(spawn), trigger_radius);
		item_table_[index] = true;
	}
}
