#pragma once
#include "BaseEnemy.h"

enum class BossZombieAttackType {
	PUNCH_ATTACK,
	JUMP_ATTACK
};

class BossZombie : public BaseEnemy
{
public:
	virtual void OnInit(entt::registry& registry) override;
	virtual void SetBehaviorTree(const vector<XMVECTOR>& target_poses) override;

public:
	void PunchAttack();
	void JumpAttack();

public:
	BossZombieAttackType cur_attack_type_ = BossZombieAttackType::PUNCH_ATTACK;
};
