#pragma once
#include "BaseEnemy.h"

class BossZombie : public BaseEnemy
{
public:
	virtual void OnInit(entt::registry& registry) override;
	virtual void SetBehaviorTree(const vector<XMVECTOR>& target_poses) override;

public:
	virtual void Attack() override;
};
