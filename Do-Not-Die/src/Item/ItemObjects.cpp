#include "ItemObjects.h"

void MedicalBoxItem::OnCreate()
{
	item_icon_ = "MedicalBox.png";
	item_count_ = 0;
	item_cooltime_ = 5.0f;

	heal_amount = 30.0f;
}

void MedicalBoxItem::UseItem()
{
	float added_hp = min(100, owner_->GetCurHp() + heal_amount);
	owner_->SetCurHp(added_hp);
}

void HealKitItem::OnCreate()
{
	item_icon_ = "HealKit.png";
	item_count_ = 0;
	item_cooltime_ = 3.0f;

	heal_amount = 15.0f;
}

void HealKitItem::UseItem()
{
	float added_hp = min(100, owner_->GetCurHp() + heal_amount);
	owner_->SetCurHp(added_hp);
}

void EnergyDrinkItem::OnCreate()
{
	item_icon_ = "EnergyDrink.png";
	item_count_ = 0;
	item_cooltime_ = 15.0f;
}

void EnergyDrinkItem::UseItem()
{

}

void DrugItem::OnCreate()
{
	item_icon_ = "Drug.png";
	item_count_ = 0;
	item_cooltime_ = 20.0f;
}

void DrugItem::UseItem()
{
}

void ARAmmoItem::OnCreate()
{
	item_icon_ = "AR_Ammo.png";
	item_count_ = 0;
	item_cooltime_ = 0.0f;
}

void ARAmmoItem::UseItem()
{
	owner_->cur_weapon_total_remained_[(int)EQUIPPED_WEAPON::AUTO_RIFLE] += AUTO_RIFLE_MAX;
}

void PistolAmmoItem::OnCreate()
{
	item_icon_ = "Pistol_Ammo.png";
	item_count_ = 0;
	item_cooltime_ = 0.0f;
}

void PistolAmmoItem::UseItem()
{
	owner_->cur_weapon_total_remained_[(int)EQUIPPED_WEAPON::HAND_GUN] += HAND_GUN_MAX;
}

void GrenadeItem::OnCreate()
{
	item_icon_ = "Grenade.png";
	item_count_ = 0;
	item_cooltime_ = 0.0f;
}

void GrenadeItem::UseItem()
{
	owner_->cur_weapon_using_remained_[(int)EQUIPPED_WEAPON::GRENADE] += 1;
}

void RepairPartItem::OnCreate()
{
	item_icon_ = "RepairPart.png";
	item_count_ = 0;
	item_cooltime_ = 0.0f;
}

void RepairPartItem::UseItem()
{
}
