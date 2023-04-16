#pragma once
#include "Engine_include.h"

namespace reality
{
	class TriggerEvent : public Event
	{
	public:
		TriggerEvent(entt::entity target_actor, entt::entity trigger_actor, bool begin_or_end);

		virtual void Process() override;

	private:
		entt::entity target_actor_;
		entt::entity trigger_actor_;
		bool begin_or_end_;
	};
}
