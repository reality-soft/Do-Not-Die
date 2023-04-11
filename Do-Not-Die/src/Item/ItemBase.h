#pragma once
#include "Engine_include.h"
#include "Player.h"

class ItemBase
{
public:
	string item_icon_;
	int item_count_;
protected:
	Player* owner_;
public:
	virtual void OnCreate() = 0;
	virtual void Use() = 0;
	void SetOwner(Player* player); 
};

