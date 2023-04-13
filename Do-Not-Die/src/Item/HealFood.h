#pragma once
#include "ItemBase.h"

class HealFood : public ItemBase
{
private:
	float heal_amount = 15.0f;
public:
	void OnCreate() override;
	void UseItem() override;
};

