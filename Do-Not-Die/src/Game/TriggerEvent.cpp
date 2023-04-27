#include "TriggerEvent.h"
#include "ItemObjects.h"
#include "Grenade.h"
using namespace reality;

TriggerEvent::TriggerEvent(entt::entity target_actor, entt::entity trigger_actor, bool is_begin)
{
	target_actor_ = target_actor;
	trigger_actor_ = trigger_actor;
	is_begin_ = is_begin;

	if (SCENE_MGR->GetActor<Player>(target_actor_) != nullptr)
		is_actor_player_ = true;
	else
		is_actor_player_ = false;

	if (SCENE_MGR->GetActor<Enemy>(target_actor_) != nullptr)
		is_actor_zombie_ = true;
	else
		is_actor_zombie_ = false;


	auto trigger_component = SCENE_MGR->GetScene(INGAME)->GetRegistryRef().try_get<C_TriggerVolume>(trigger_actor_);
	if (trigger_component == nullptr)
		return;

	if (trigger_component->tag == "item")
	{
		trigger_type_ = TriggerType::ITEM_TO_PLAYER;
	}
	if (trigger_component->tag == "extract")
	{
		trigger_type_ = TriggerType::REPAIR_PART_EXTRACT;
	}
	if (trigger_component->tag == "repair")
	{
		trigger_type_ = TriggerType::CAR_REPAIR;
	}
	if (trigger_component->tag == "defense")
	{
		trigger_type_ = TriggerType::CAR_DEFENSE;
	}
	if (trigger_component->tag == "grenade")
	{
		trigger_type_ = TriggerType::GRENADE_EXPLOSION;
	}
}

void TriggerEvent::Process()
{
	PlayerProcess();
	ZombiePeocess();
}

void reality::TriggerEvent::PlayerProcess()
{
	if (is_actor_player_ == false)
		return;

	switch (trigger_type_)
	{
	case TriggerType::ITEM_TO_PLAYER:
		if (is_begin_)
		{
			auto item = SCENE_MGR->GetActor<Item>(trigger_actor_);
			if (item != nullptr)
				PlayerSelectable(item, true);			
		}
		else
		{
			auto item = SCENE_MGR->GetActor<Item>(trigger_actor_);
			if (item != nullptr)
				PlayerSelectable(item, false);			
		}
		break;

	case TriggerType::REPAIR_PART_EXTRACT:
		if (is_begin_)
			PlayerCanExtract(true);		
		else
			PlayerCanExtract(false);
		break;

	case TriggerType::CAR_REPAIR:
		if (is_begin_)
			PlayerCanRepair(true);
		else
			PlayerCanRepair(false);
		break;

	case TriggerType::CAR_DEFENSE:
		if (is_begin_)
			PlayerDefense(true);
		else
			PlayerDefense(false);
		break;
	}
}

void reality::TriggerEvent::ZombiePeocess()
{
	if (is_actor_zombie_ == false)
		return;

	switch (trigger_type_)
	{
	case TriggerType::CAR_DEFENSE:
		if (is_begin_)
			ZombieDefense(true);
		else
			ZombieDefense(false);
		break;
	case TriggerType::GRENADE_EXPLOSION:
		auto grenade = SCENE_MGR->GetActor<Grenade>(trigger_actor_);
		GrenadeExplosion(SCENE_MGR->GetActor<Enemy>(target_actor_), grenade->GetRange(), grenade->GetDamage());
		break;
	}
}

void reality::TriggerEvent::PlayerSelectable(Item* item_actor, bool selectable)
{
	// ITEM TEST CODE
	auto player = SCENE_MGR->GetPlayer<Player>(0);
	if (player == nullptr)
		return;	
	
	if (selectable)
	{
		for (const auto& item : player->selectable_items_)
		{
			if (item.second == item_actor)
				return;
		}
		float distance = Distance(player->GetCurPosition(), item_actor->GetCurPosition());
		player->selectable_items_.insert(make_pair(distance, item_actor));
		++player->selectable_counts_;
	}
	else
	{
		auto iter = player->selectable_items_.begin();
		for (const auto& item : player->selectable_items_)
		{
			if (item.second == item_actor)
			{
				player->selectable_items_.erase(iter);
				--player->selectable_counts_;
				break;
			}
			++iter;
		}
	}
}

void reality::TriggerEvent::PlayerCanExtract(bool can_extract)
{
	auto player = SCENE_MGR->GetPlayer<Player>(0);
	if (player == nullptr)
		return;

	player->can_extract_repair_ = can_extract;
	player->repair_extract_trigger = trigger_actor_;
}

void reality::TriggerEvent::PlayerCanRepair(bool can_repair)
{
	auto player = SCENE_MGR->GetPlayer<Player>(0);
	if (player == nullptr)
		return;

	player->can_repair_car_ = can_repair;
}

void reality::TriggerEvent::ZombieDefense(bool can_defense)
{
	auto zombie = SCENE_MGR->GetActor<Enemy>(target_actor_);
	if (zombie == nullptr)
		return;

	zombie->in_defense_bound_ = can_defense;

}

void reality::TriggerEvent::PlayerDefense(bool can_defense)
{
	auto player = SCENE_MGR->GetPlayer<Player>(0);
	if (player == nullptr)
		return;

	player->player_in_defense_ = can_defense;
}

void reality::TriggerEvent::GrenadeExplosion(Enemy* enemy_actor, float range, float damage)
{
	auto grenade = SCENE_MGR->GetActor<Grenade>(trigger_actor_);
	float distance = XMVectorGetX(XMVector3Length(grenade->GetCurPosition() - enemy_actor->GetCurPosition()));
	
	float weight = range / distance;
	weight = min(1.0f, weight);
	weight = 1.0f - weight;
	
	enemy_actor->TakeDamage(damage * weight);
}
