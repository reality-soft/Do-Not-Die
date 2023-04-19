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

		float countdown_timer_ = 0;
		UINT wave_count_ = 0;

		void CreateExtractPoints(entt::registry& reg);
		void PlayerExtractRepair();
		void SpawnRepairItem(XMFLOAT3 position);
		void DeleteExtractPoint(entt::entity ent);
	private:
		void RandomSpawnItem(float trigger_radius);

	private:

		shared_ptr<Environment> world_env_;
		GuideLine item_spawns_;
		GuideLine repair_spawns_;
		map<UINT, bool> item_table_;
		set<entt::entity> repair_parts;
	};
}

