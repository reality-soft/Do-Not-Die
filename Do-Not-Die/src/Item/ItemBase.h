#pragma once
#include "Engine_include.h"
#include "Item.h"
#include "Player.h"

class ItemBase
{
public:
	int		item_count_;
	float	item_cooltime_;
	string	item_icon_;
	Player* owner_;
	ItemType item_type_;
protected:
	virtual void UseItem() = 0;
public:
	virtual void OnCreate() = 0;
	void	Use();
	void	Drop();
	void	SetOwner(Player* player); 
	void	AddCount(int count = 1);
	int		GetCount() { return item_count_; }
	float	GetCooltime() { return item_cooltime_; }
	string	GetIcon() { return item_icon_; }
};

