#pragma once
#include "Engine_include.h"
#include "Player.h"
#include "Item.h"

namespace reality
{
	enum class TriggerType
	{
		ITEM_TO_PLAYER,
		REPAIR_PART_EXTRACT,
		ZOMBIE_ACTIVITY,
	};

	class TriggerEvent : public Event
	{
	public:
		TriggerEvent(entt::entity target_actor, entt::entity trigger_actor, bool is_begin);

		virtual void Process() override;

	private:
		entt::entity trigger_actor_;
		entt::entity target_actor_;
		TriggerType trigger_type_;
		bool is_begin_;

		void PlayerSelectable(Item* item_actor, bool selectable);
		void PlayerCanExtract(bool can_extract);
	};
}
