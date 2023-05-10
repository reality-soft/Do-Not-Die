#pragma once
#include "BaseEnemy.h"

class NormalZombie : public BaseEnemy
{
public:
	virtual void OnInit(entt::registry& registry) override;

public:
	virtual void Attack() override;

public:
	void AddImpulse(XMVECTOR direction, float strength);

public:
	virtual void SetBehaviorTree(const vector<XMVECTOR>& target_poses) override;
	void SetMeshId(const string& mesh_id);

private:
	string mesh_id_;
	int cur_node = 0;
};
