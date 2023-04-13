#include "HealFood.h"

void HealFood::OnCreate()
{
	item_icon_ = "T_HealFood.png";
	item_count_ = 0;
	item_cooltime_ = 3.0f;

	heal_amount = 15.0f;
}

void HealFood::UseItem()
{
	float added_hp = min(100, owner_->GetCurHp() + heal_amount);
	owner_->SetCurHp(added_hp);
}
