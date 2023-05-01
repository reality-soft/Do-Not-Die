#pragma once
#include "Engine_include.h"

using namespace reality;

class RidingCar : public Actor
{
public:
	virtual void OnInit(entt::registry& registry) override;
	virtual void OnUpdate() override;

private:
	GuideLine ride_track;
};

