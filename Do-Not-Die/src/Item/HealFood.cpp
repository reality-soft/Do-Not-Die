#include "HealFood.h"

void HealFood::OnCreate()
{
	item_icon_ = "T_HealFood.png";
	item_count_ = 0;
}

void HealFood::UseItem()
{
	float added_hp = min(100, owner_->GetCurHp() + heal_amount);
	owner_->SetCurHp(added_hp);
}
