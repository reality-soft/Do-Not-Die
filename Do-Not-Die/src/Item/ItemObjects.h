#pragma once
#include "ItemBase.h"

class MedicalBoxItem : public ItemBase
{
private:
	float heal_amount;
public:
	void OnCreate() override;
	void UseItem() override;
};

class HealKitItem : public ItemBase
{
private:
	float heal_amount;
public:
	void OnCreate() override;
	void UseItem() override;
};

class EnergyDrinkItem : public ItemBase
{
private:
	float speed_increase;
public:
	void OnCreate() override;
	void UseItem() override;
};

class DrugItem : public ItemBase
{
private:
	float damage_increase;
public:
	void OnCreate() override;
	void UseItem() override;
};

class VaccineItem : public ItemBase
{
private:
	float infection_reduce;

public:
	void OnCreate() override;
	void UseItem() override;
};

class ARAmmoItem : public ItemBase
{
private:
	float rifle_fills;
public:
	void OnCreate() override;
	void UseItem() override;
};

class PistolAmmoItem : public ItemBase
{
private:
	float pistol_fills;
public:
	void OnCreate() override;
	void UseItem() override;
};

class GrenadeItem : public ItemBase
{
private:
	float grenade_fills;
public:
	void OnCreate() override;
	void UseItem() override;
};

class RepairPartItem : public ItemBase
{
public:
	int repair_influence = 1;
	void OnCreate() override;
	void UseItem() override;
};
