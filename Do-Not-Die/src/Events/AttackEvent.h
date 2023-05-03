#pragma once
#include "GameEvents.h"
#include "Shape.h"

class AttackEvent_SingleRay : public Event
{
public:
	AttackEvent_SingleRay(RayShape _ray, entt::entity actor_id) : ray(_ray), actor_id_(actor_id) {};
	virtual void Process() override;

	void EnemyProcess();
	void PlayerProcess();

private:
	RayShape ray;
	entt::entity actor_id_;
};

class AttackEvent_BoundSphere : public Event
{
public:
	AttackEvent_BoundSphere(float damage, SphereShape sphere, entt::entity actor_id) : sphere_(sphere), actor_id_(actor_id), damage_(damage){};
	virtual void Process() override;

	void EnemyProcess();
	void PlayerProcess();

private:
	float damage_;
	SphereShape sphere_;
	entt::entity actor_id_;
	vector<entt::entity> hit_actors_;
};
