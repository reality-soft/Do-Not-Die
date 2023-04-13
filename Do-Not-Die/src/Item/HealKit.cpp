#include "HealKit.h"

void HealKit::OnCreate()
{
	item_icon_ = "T_HealKit.png"; 
	item_count_ = 0;
	item_cooltime_ = 5.0f; 
	
	heal_amount = 30.0f;
}

void HealKit::UseItem()
{
	float added_hp = min(100, owner_->GetCurHp() + heal_amount);
	owner_->SetCurHp(added_hp);
}
