#pragma once
#include "Engine_include.h"

namespace reality
{
	class WaveSystem : public System
	{
	public:
		virtual void OnCreate(entt::registry& reg) override;
		virtual void OnUpdate(entt::registry& reg) override;

		void SetWorldEnv(Environment* env);
		void CreateExtractPoints(entt::registry& reg);

		float countdown_timer_ = 0;
		UINT wave_count_ = 0;
		int car_repaired = 0;
		float car_health = 100.0f;

	private:
		void RandomSpawnItem(float trigger_radius);
		void CreateCarEventTriggers(XMFLOAT3 point, float repair_radius, float zombie_target_radius);
		void PlayerExtractRepair();
		void PlayerRepairCar();
		void SpawnRepairItem(XMFLOAT3 position);
		void DeleteExtractPoint(entt::entity ent);
		void SpawnZombies(float interval, UINT count);

		void WaveStart();
		void WaveFinish();
	private:
		bool zombie_spawn_ = false;
		UINT zombie_spawn_count_ = 0;

		shared_ptr<Environment> world_env_;
		vector<GuideLine>* zomebie_tracks_;
		GuideLine item_spawns_;
		GuideLine repair_spawns_;
		GuideLine car_event_;
		map<UINT, bool> item_table_;
		set<entt::entity> repair_parts;

		vector<std::string> enemy_meshes = {
		"Zombie_Businessman_Male_01.skmesh",
		"Zombie_Cheerleader_Female_01.skmesh",
		"Zombie_Daughter_Female_01.skmesh",
		"Zombie_Police_Male_01.skmesh",
		"Zombie_RiotCop_Male_01.skmesh",
		"Zombie_SchoolGirl_Female_01.skmesh",
		"Zombie_Bellboy_Male_01.skmesh",
		};
	};
}

