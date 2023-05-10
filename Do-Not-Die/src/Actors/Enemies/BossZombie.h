#pragma once
#include "BaseEnemy.h"

enum class BossZombieAttackType {
	PUNCH_ATTACK
};

class BossZombie : public BaseEnemy
{
public:
	virtual void OnInit(entt::registry& registry) override;
	virtual void SetBehaviorTree(const vector<XMVECTOR>& target_poses) override;

public:
	virtual void Attack() override;

public:
	BossZombieAttackType cur_attack_type = BossZombieAttackType::PUNCH_ATTACK;
};
