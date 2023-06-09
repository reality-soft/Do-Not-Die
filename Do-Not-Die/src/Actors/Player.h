#pragma once
#include "Engine_include.h"
#include "GameCharacter.h"
#include "Item.h"

#define INVENTORY_MAX 4

class ItemBase;

using namespace reality;

#define AUTO_RIFLE_MAX 30
#define HAND_GUN_MAX 8

enum class EQUIPPED_WEAPON {
	AUTO_RIFLE,
	HAND_GUN,
	MELEE_WEAPON,
	GRENADE,
	NUM_OF_WEAPON_TYPE
};

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
	void Roll();
	void Jump();
	void Attack();
	void Aim(bool active);
	void Reload();
	void ThrowGrenade();
	void MeeleAttack();
	void PickClosestItem();
	void SetCharacterMovementAnimation();

	void IncreaseInfection();
	virtual void TakeDamage(int damage) override;

public:
	bool IsAiming();
	bool IsReloading();
	void InteractionRotate(XMVECTOR direction);

public:
	void ResetPos();
	void SetPos(const XMVECTOR& position = { 0.f, 100.f, 0.f, 0.f });
	int  GetKillScore() { return kill_score_; }
	void AddKillScore() { kill_score_++; }

private:
	void AddFlashLight();
	void UpdateFlashLight();

private:
	void CalculateMovementAngle();
	void ChangeWeapon();

private:
	bool is_aiming_ = false;
public:
	int cur_weapon_using_remained_[static_cast<int>(EQUIPPED_WEAPON::NUM_OF_WEAPON_TYPE)] = { 30, 8, 1, 5 };
	int cur_weapon_total_remained_[static_cast<int>(EQUIPPED_WEAPON::NUM_OF_WEAPON_TYPE)] = { 60, 16, 0, 0 };
	string socket_ids_[static_cast<int>(EQUIPPED_WEAPON::NUM_OF_WEAPON_TYPE)];
	string stm_ids_[static_cast<int>(EQUIPPED_WEAPON::NUM_OF_WEAPON_TYPE)];
	XMMATRIX stm_local_[static_cast<int>(EQUIPPED_WEAPON::NUM_OF_WEAPON_TYPE)];
public:
	int kill_score_ = 0;
public:
	EQUIPPED_WEAPON cur_equipped_weapon_ = EQUIPPED_WEAPON::HAND_GUN;
	bool is_attacking_ = false;
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

	float extract_during_time_ = 0.0f;
	float repair_during_time_ = 0.0f;

	const float extract_time_takes_ = 5.0f;
	const float repair_time_takes_ = 5.0f;

	bool can_extract_repair_ = false;
	bool can_repair_car_ = false;

	bool is_reloading_ = false;
	//bool is_hit_ = false;
	bool roll_ = false;
	bool is_rolling_ = false;
	bool is_dead_ = false;
	
	entt::entity repair_extract_trigger;

	void SetSpawnPoint(XMVECTOR point);
	XMVECTOR spawn_point;
	bool controller_enable_ = true;
	bool player_in_defense_ = false;

	int infection_probability_ = 0;
	int hit_count_ = 0;
	bool is_infected_ = false;
	bool is_zombie_ = false;

private:
	void UpdateTimer();
};