#pragma once
#include "Engine_include.h"
#include "GameCharacter.h"
#include "Item.h"

#define INVENTORY_MAX 4

class ItemBase;

using namespace reality;

class Player : public GameCharacter
{
public:
	virtual void OnInit(entt::registry& registry) override;
	virtual void OnUpdate() override;
public:
	void MoveRight();
	void MoveLeft();
	void MoveForward();
	void MoveBack();
	void Jump();
	void Fire();
	void Aim(bool active);
	void ThrowGrenade();
	void PickClosestItem();
	void SetCharacterMovementAnimation();

public:
	bool IsAiming();
	void InteractionRotate(XMVECTOR direction);

public:
	void ResetPos();
	void SetPos(const XMVECTOR& position = { 0.f, 100.f, 0.f, 0.f });

public:
	virtual float GetMaxHp() const override;
	virtual void SetCurHp(int hp) override;
	virtual void TakeDamage(int damage) override;
	virtual float GetCurHp() const override;

private:
	void AddFlashLight();
	void UpdateFlashLight();

private:
	void CalculateMovementAngle();

private:
	bool is_aiming_ = false;

public:
	bool is_firing_ = false;
	float angle_ = 0.0f;
	XMVECTOR direction_;

private:
	float grenade_cooltime_ = 3.0f;
	float grenade_timer_ = 0.0f;

private:
	vector<shared_ptr<ItemBase>>	inventory_;
	vector<float>					inventory_timer_;

public:
	map<float, Item*> selectable_items_;
	UINT selectable_counts_ = 0;
	int selected_slot = 0;
	bool AcquireItem(shared_ptr<ItemBase> item);
	void SelectSlot(int slot);
	void UseItem();
	void DropItem();
	bool HasRepairPart();
	void UseRepairPart();
	vector<shared_ptr<ItemBase>>&	GetInventory();
	vector<float>&					GetInventoryTimer();

	float drop_during_time_ = 0.0f;
	float extract_during_time_ = 0.0f;
	float repair_during_time_ = 0.0f;

	const float drop_time_takes_ = 0.5f;
	const float extract_time_takes_ = 5.0f;
	const float repair_time_takes_ = 5.0f;

	bool can_extract_repair_ = false;
	bool can_repair_car_ = false;
	
	entt::entity repair_extract_trigger;

	void SetSpawnPoint(XMVECTOR point);
	XMVECTOR spawn_point;
	bool controller_enable_ = true;
	bool player_in_defense_ = false;

private:
	void UpdateTimer();
};