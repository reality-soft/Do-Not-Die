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

	private:
		void RandomSpawnItem(float trigger_radius);

	private:

		shared_ptr<Environment> world_env_;
		vector<GuideLine>* item_spawns_;
		vector<GuideLine>* repair_spawns_;
	};
}

