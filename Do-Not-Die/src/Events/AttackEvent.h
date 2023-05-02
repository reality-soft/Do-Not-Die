#pragma once
#include "GameEvents.h"
#include "Shape.h"

class AttackEvent_SingleRay : public Event
{
public:
	AttackEvent_SingleRay(RayShape _ray, entt::entity actor_id) : ray(_ray), actor_id_(actor_id) {};
	virtual void Process() override;

private:
	RayShape ray;
	entt::entity actor_id_;
};

class AttackEvent_BoundSphere : public Event
{
public:
	AttackEvent_BoundSphere(SphereShape sphere, entt::entity actor_id) : sphere_(sphere), actor_id_(actor_id) {};
	virtual void Process() override;

	void EnemyProcess();
	void PlayerProcess();

private:
	SphereShape sphere_;
	entt::entity actor_id_;
};
