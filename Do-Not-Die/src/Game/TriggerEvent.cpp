#include "TriggerEvent.h"
#include "ItemObjects.h"

using namespace reality;

TriggerEvent::TriggerEvent(entt::entity target_actor, entt::entity trigger_actor, bool is_begin)
{
	target_actor_ = target_actor;
	trigger_actor_ = trigger_actor;
	is_begin_ = is_begin;

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
}

void TriggerEvent::Process()
{
	switch (trigger_type_)
	{
	case TriggerType::ITEM_TO_PLAYER:
		if (is_begin_)
		{
			auto item = SCENE_MGR->GetActor<Item>(trigger_actor_);
			if (item == nullptr)
				return;
			PlayerSelectable(item, true);
		}
		else
		{
			auto item = SCENE_MGR->GetActor<Item>(trigger_actor_);
			if (item == nullptr)
				return;
			PlayerSelectable(item, false);
		}
		break;

	case TriggerType::REPAIR_PART_EXTRACT:	
		if (is_begin_)
		{
			PlayerCanExtract(true);
		}
		else
		{
			PlayerCanExtract(false);
		}

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
		float distance = Distance(player->GetTransformMatrix().r[3], item_actor->GetTransformMatrix().r[3]);
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

	//switch (item_type)
	//{
	//case ItemType::eMedicalBox:
	//{
	//	shared_ptr<MedicalBoxItem> medical_box = make_shared<MedicalBoxItem>();
	//	medical_box->OnCreate();
	//	medical_box->AddCount(1);
	//	player->AcquireItem(medical_box);
	//	break;
	//}
	//case ItemType::eHealKit:
	//{
	//	shared_ptr<HealKitItem> heal_kit = make_shared<HealKitItem>();
	//	heal_kit->OnCreate();
	//	heal_kit->AddCount(1);
	//	player->AcquireItem(heal_kit);
	//	break;
	//}
	//case ItemType::eEnergyDrink:
	//{
	//	shared_ptr<EnergyDrinkItem> energy_drink = make_shared<EnergyDrinkItem>();
	//	energy_drink->OnCreate();
	//	energy_drink->AddCount(1);
	//	player->AcquireItem(energy_drink);
	//	break;
	//}
	//case ItemType::eDrug:
	//{
	//	shared_ptr<DrugItem> drug = make_shared<DrugItem>();
	//	drug->OnCreate();
	//	drug->AddCount(1);
	//	player->AcquireItem(drug);
	//	break;
	//}
	//case ItemType::eAR_Ammo:
	//{
	//	shared_ptr<ARAmmoItem> ar_ammo = make_shared<ARAmmoItem>();
	//	ar_ammo->OnCreate();
	//	ar_ammo->AddCount(1);
	//	player->AcquireItem(ar_ammo);
	//	break;
	//}
	//case ItemType::ePistol_Ammo:
	//{
	//	shared_ptr<PistolAmmoItem> pistol_ammo = make_shared<PistolAmmoItem>();
	//	pistol_ammo->OnCreate();
	//	pistol_ammo->AddCount(1);
	//	player->AcquireItem(pistol_ammo);
	//	break;
	//}
	//case ItemType::eGrenade:
	//{
	//	shared_ptr<GrenadeItem> grenade = make_shared<GrenadeItem>();
	//	grenade->OnCreate();
	//	grenade->AddCount(1);
	//	player->AcquireItem(grenade);
	//	break;
	//}
	
}

void reality::TriggerEvent::PlayerCanExtract(bool can_extract)
{
	auto player = SCENE_MGR->GetPlayer<Player>(0);
	if (player == nullptr)
		return;

	player->can_extract_repair = can_extract;
	player->repair_extract_trigger = trigger_actor_;
}
