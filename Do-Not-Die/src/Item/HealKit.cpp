#include "HealKit.h"

void HealKit::OnCreate()
{
	item_icon_ = "T_HealKit.png"; 
	item_count_ = 0;
}

void HealKit::Use()
{
	if (owner_ == nullptr)
		return;

	if (item_count_ <= 0)
		return;

	float added_hp = max(100, owner_->GetCurHp() + heal_amount);
	owner_->SetCurHp(added_hp);
}
