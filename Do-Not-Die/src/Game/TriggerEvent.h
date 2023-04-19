#pragma once
#include "Engine_include.h"
#include "Player.h"
#include "Item.h"

namespace reality
{
	enum class TriggerType
	{
		ITEM_TO_PLAYER,
		ZOMBIE_ACTIVITY,
	};

	class TriggerEvent : public Event
	{
	public:
		TriggerEvent(entt::entity target_actor, entt::entity trigger_actor, bool begin_or_end);

		virtual void Process() override;

	private:
		entt::entity target_actor_;
		entt::entity trigger_actor_;
		bool begin_or_end_;
		TriggerType trigger_type_;
		Item* item_actor_ = nullptr;

		void PlayerSelectable(Item* item_actor, bool selectable);
	};
}
