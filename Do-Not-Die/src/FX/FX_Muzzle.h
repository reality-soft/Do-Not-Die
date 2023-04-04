#pragma once
#include "FX_BaseEffectActor.h"

namespace reality
{
	class FX_Muzzle : public FX_BaseEffectActor
	{
	public:
		virtual void OnInit(entt::registry& registry);
	};
}

