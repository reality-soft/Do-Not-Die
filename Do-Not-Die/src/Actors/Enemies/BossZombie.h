#pragma once
#include "BaseEnemy.h"

enum class BossZombieAttackType {
	LEFT_RIGHT_HOOK,
	KICK_ATTACK,
	PUNCH_ATTACK,
	JUMP_ATTACK
};

class BossZombie : public BaseEnemy
{
public:
	virtual void OnInit(entt::registry& registry) override;
	virtual void SetBehaviorTree(const vector<XMVECTOR>& target_poses) override;

public:
	void LeftHook();
	void RightHook();
	void KickAttack();
	void PunchAttack();
	void JumpAttack();

	SphereShape CreateFrontAttackSphere(float radius);

public:
	BossZombieAttackType cur_attack_type_ = BossZombieAttackType::PUNCH_ATTACK;
};
