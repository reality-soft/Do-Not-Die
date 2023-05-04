#pragma once
#include "FX_BaseEffectActor.h"

namespace reality
{
	class FX_MeleeImpact : public FX_BaseEffectActor
	{
	public:
		virtual void OnInit(entt::registry& registry);
	};
}

