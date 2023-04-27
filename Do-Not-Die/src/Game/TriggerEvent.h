#pragma once
#include "Engine_include.h"
#include "Player.h"
#include "Enemy.h"
#include "Item.h"

namespace reality
{
	enum class TriggerType
	{
		ITEM_TO_PLAYER,
		REPAIR_PART_EXTRACT,
		CAR_REPAIR,
		CAR_DEFENSE,
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
		bool is_actor_player_;
		bool is_actor_zombie_;

		void PlayerProcess();
		void ZombiePeocess();

		void PlayerSelectable(Item* item_actor, bool selectable);
		void PlayerCanExtract(bool can_extract);
		void PlayerCanRepair(bool can_repair);
		void PlayerDefense(bool can_defense);
		void ZombieDefense(bool can_defense);
	};
}
