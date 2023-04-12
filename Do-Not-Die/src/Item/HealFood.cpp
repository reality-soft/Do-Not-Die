#include "HealFood.h"

void HealFood::OnCreate()
{
	item_icon_ = "";
	item_count_ = 0;
}

void HealFood::Use()
{
	if (owner_ == nullptr)
		return;

	if (item_count_ <= 0)
		return;

	float added_hp = max(100, owner_->GetCurHp() + heal_amount);
	owner_->SetCurHp(added_hp);
}
