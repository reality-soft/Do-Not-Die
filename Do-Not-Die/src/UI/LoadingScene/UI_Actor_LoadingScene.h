#pragma once
#include "UIActor.h"

namespace reality
{
	class UI_Actor_LoadingScene : public UIActor
	{
	public:
		virtual void OnInit(entt::registry& registry);
		virtual void OnUpdate();
	private:
		shared_ptr<UI_Image>	background_;
	private:
		void CreateUI();
		void UpdateUI();
	};

}

