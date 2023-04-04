#pragma once
#include "UIActor.h"

namespace reality
{
	class UI_Actor_StartScene : public UIActor
	{
	public:
		virtual void OnInit(entt::registry& registry);
		virtual void OnUpdate();
	private:
		shared_ptr<UI_Image>	background_;
		shared_ptr<UI_Button>	newgame_button_;
		shared_ptr<UI_Button>	loadinggame_button;
		shared_ptr<UI_Button>	option_button_;
		shared_ptr<UI_Button>	exit_button_;
	private:
		void CreateUI();
		void UpdateUI();
	};

}

