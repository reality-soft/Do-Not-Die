#pragma once
#include "FX_BaseEffectActor.h"

namespace reality
{
	class FX_Range : public FX_BaseEffectActor
	{
	public:
		virtual void OnInit(entt::registry& registry) override;
		virtual void OnUpdate() override;
	};
}

