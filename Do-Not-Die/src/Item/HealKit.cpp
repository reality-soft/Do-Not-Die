#include "HealKit.h"

void HealKit::OnCreate()
{
	item_icon_ = "T_HealKit.png"; 
	item_count_ = 0;
}

void HealKit::UseItem()
{
	float added_hp = min(100, owner_->GetCurHp() + heal_amount);
	owner_->SetCurHp(added_hp);
}
