#pragma once
#include "Engine_include.h"
#include "StaticShadows.h"

namespace reality
{
	class WaveSystem : public System
	{
	public:
		virtual void OnCreate(entt::registry& reg) override;
		virtual void OnUpdate(entt::registry& reg) override;

		void SetWorldEnv(Environment* env);

		float countdown_timer_ = 0;
		UINT wave_count_ = 1;
		int car_health = 40;
		int car_repair_count = 0;

	private:
		void CreateExtractPoints(entt::registry& reg);
		void CreateCarEventTriggers(XMFLOAT3 point, float repair_radius, float zombie_target_radius);
		void CreateStaticEffects();

		void RandomSpawnItem(float trigger_radius);
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
		GuideLine fx_car_fire_;
		GuideLine fx_corpse_fire_;
		map<UINT, bool> item_table_;
		set<entt::entity> repair_parts;

		set<entt::entity> car_fires_;
		set<entt::entity> corpse_fires_;

		vector<std::string> enemy_meshes = {
		"Zombie_Businessman_Male_01.skmesh",
		"Zombie_Cheerleader_Female_01.skmesh",
		"Zombie_Police_Male_01.skmesh",
		"Zombie_RiotCop_Male_01.skmesh",
		"Zombie_SchoolGirl_Female_01.skmesh",
		"Zombie_Bellboy_Male_01.skmesh",
		};
	};
}

