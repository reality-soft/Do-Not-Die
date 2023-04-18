#pragma once
#include "Actor.h"

class Grenade : public reality::Actor
{
public:
	virtual void OnInit(entt::registry& registry);
	virtual void OnUpdate();
public:
	void SetPos(FXMVECTOR pos);
	void SetDir(FXMVECTOR dir, float speed);
private:
	XMFLOAT3	dir_;
	bool		exploded_;
	float		timer_;
	float		explosion_time_;
};

