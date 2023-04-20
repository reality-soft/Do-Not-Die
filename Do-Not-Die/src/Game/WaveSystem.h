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

	private:
		void RandomSpawnItem(float trigger_radius);
		void CreateCarEventTriggers(XMFLOAT3 point, float repair_radius, float zombie_target_radius);
		void PlayerExtractRepair();
		void PlayerRepairCar();
		void SpawnRepairItem(XMFLOAT3 position);
		void DeleteExtractPoint(entt::entity ent);

	private:

		shared_ptr<Environment> world_env_;
		GuideLine item_spawns_;
		GuideLine repair_spawns_;
		GuideLine car_event_;
		map<UINT, bool> item_table_;
		set<entt::entity> repair_parts;
		int car_repaired = 0;
	};
}

