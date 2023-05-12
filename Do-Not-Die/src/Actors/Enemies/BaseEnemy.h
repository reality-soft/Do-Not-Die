#pragma once
#include "GameCharacter.h"
using namespace reality;

class BaseEnemy : public GameCharacter
{
public:
	virtual void OnUpdate() override;
	virtual void SetBehaviorTree(const vector<XMVECTOR>& target_poses) {};
	
private:
	void ChasePlayer();

public:
	void AddImpulse(XMVECTOR direction, float strength);

public:
	void SetMovement(const XMVECTOR& direction);
	virtual void Attack() {};

public:
	bool is_moving_ = false;
	bool is_attacking_ = false;
	bool is_attack_ended_ = false;
	bool in_defense_bound_ = false;
	bool player_in_sight_ = true;
	int* targeting_car_health = nullptr;

protected:
	reality::BehaviorTree behavior_tree_;

public:
	float attack_distance_ = 50.0f;
};
