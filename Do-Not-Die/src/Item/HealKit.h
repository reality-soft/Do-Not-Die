#pragma once
#include "ItemBase.h"

class HealKit : public ItemBase
{
private:
	float heal_amount = 30.0f;
public:
	void OnCreate() override;
	void UseItem() override;
};

