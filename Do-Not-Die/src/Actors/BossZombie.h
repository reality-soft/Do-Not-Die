#pragma once
#include "GameCharacter.h"

using namespace reality;

class BossZombie : public GameCharacter
{
	virtual void OnInit(entt::registry& registry) override;
	virtual void OnUpdate() override;
};

