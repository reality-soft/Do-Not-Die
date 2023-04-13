#include "ItemBase.h"

void ItemBase::Use()
{
	if (owner_ == nullptr)
		return;

	if (item_count_ <= 0)
		return;

	UseItem();

	item_count_ = max(0, item_count_ - 1);
}

void ItemBase::SetOwner(Player* player)
{
	owner_ = player;
}

void ItemBase::AddCount(int count)
{
	item_count_ += count;
}
