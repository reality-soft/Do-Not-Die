#pragma once
#include "ItemBase.h"

class HealKit : public ItemBase
{
private:
	float heal_amount;
public:
	void OnCreate() override;
	void UseItem() override;
};

