#pragma once
#include "Actor.h"

enum class ItemType
{
	eHealKit,		// heal smaller
	eMedicalBox,	// heal bigger
	eEnergyDrink,	// move faster
	eDrug,			// stronger damage
	eAR_Ammo,		// rifle ammo (30)
	ePistol_Ammo,	// pistol ammo (8)
};

class Item : public reality::Actor
{
public:
	Item(ItemType item_type, XMFLOAT3 spawn_point, float trigger_radius);

	virtual void OnInit(entt::registry& registry);
	virtual void OnUpdate();

	reality::SphereShape trigger_sphere_;
	ItemType item_type_;
};