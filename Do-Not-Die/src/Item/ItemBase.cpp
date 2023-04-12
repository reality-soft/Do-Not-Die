#include "ItemBase.h"

void ItemBase::SetOwner(Player* player)
{
	owner_ = player;
}

void ItemBase::AddCount(int count)
{
	item_count_ += count;
}
