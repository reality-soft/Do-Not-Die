#pragma once
#include "Engine_include.h"
#include "Player.h"

class ItemBase
{
public:
	string item_icon_;
	int item_count_;
	Player* owner_;
protected:
	virtual void UseItem() = 0;
public:
	virtual void OnCreate() = 0;
	void Use();
	void SetOwner(Player* player); 
	void AddCount(int count = 1);
	int GetCount() { return item_count_; }
	string GetIcon() { return item_icon_; }
};

