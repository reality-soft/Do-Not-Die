#include "TriggerEvent.h"
#include "ItemObjects.h"

using namespace reality;

TriggerEvent::TriggerEvent(entt::entity target_actor, entt::entity trigger_actor, bool begin_or_end)
	: Event(TRIGGER)
{
	target_actor_ = target_actor;
	trigger_actor_ = trigger_actor;
	begin_or_end_ = begin_or_end;

	Item* item_actor = SCENE_MGR->GetActor<Item>(trigger_actor);
	if (item_actor)
	{
		trigger_type_ = TriggerType::ITEM_TO_PLAYER;
		item_actor_ = item_actor;
	}
	else
	{
		item_actor_ = nullptr;
	}
}

void TriggerEvent::Process()
{
	switch (trigger_type_)
	{
	case TriggerType::ITEM_TO_PLAYER:
		if (begin_or_end_)
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
