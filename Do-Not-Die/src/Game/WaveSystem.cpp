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
	{
		RandomSpawnItem(10.0f);
		i = 0;
	}

	float counting_time = world_env_->GetCountingTime();
	if (world_env_->IsDayChanged())
	{
		switch (world_env_->GetCurrentDay())
		{
		case Day::eNoon:
			countdown_timer_ = fabs(world_env_->GetTimeLimits().x * 2);
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
}

void reality::WaveSystem::RandomSpawnItem(float trigger_radius)
{
	for (const auto& item : item_spawns_->at(0).line_nodes)
	{
		const auto& spawn = item.second;

		SCENE_MGR->AddActor<Item>(ItemType::eMedicalBox, _XMFLOAT3(spawn), trigger_radius);
	}

}
