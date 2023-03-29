#pragma once
#include "PointLightActor.h"

namespace reality
{
	class TestPointLightActor : public PointLightActor
	{
	public:
		virtual void OnInit(entt::registry& registry);
		virtual void OnUpdate();
	};
}

