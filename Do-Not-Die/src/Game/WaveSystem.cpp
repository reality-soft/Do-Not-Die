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
}

void reality::WaveSystem::SetWorldEnv(Environment* env)
{
	world_env_ = shared_ptr<Environment>(env);
}

void reality::WaveSystem::RandomSpawnItem(float trigger_radius)
{
	for (const auto& item : item_spawns_->at(0).line_nodes)
	{
		const auto& spawn = item.second;

		SCENE_MGR->AddActor<Item>(ItemType::eMedicalBox, _XMFLOAT3(spawn), trigger_radius);
	}

}
