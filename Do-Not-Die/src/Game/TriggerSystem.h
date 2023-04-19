#pragma once
#include "Engine_include.h"

namespace reality
{
	class TriggerSystem : public System
	{
	public:
		virtual void OnCreate(entt::registry& reg) override;
		virtual void OnUpdate(entt::registry& reg) override;

		void AddTriggerAtActor(entt::entity ent, float radius);
		bool IsActorInTrigger(entt::entity ent, const C_TriggerVolume& trigger);

	private:
		map<entt::entity, entt::entity> current_triggers;
	};
}

