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
		C_UI* ui_comp_;
	private:
		shared_ptr<UI_Image>	background_;
		shared_ptr<UI_Button>	newgame_button_;
		shared_ptr<UI_Button>	loadinggame_button;
		shared_ptr<UI_Button>	option_button_;
		shared_ptr<UI_Button>	exit_button_;

		shared_ptr<UI_Image>	option_opaque_;
		shared_ptr<UI_Image>	option_window_;
		shared_ptr<UI_Button>	option_close_button_;
	private:
		void CreateUI();
		void UpdateUI();
	private:
		void OpenOptionWindow();
		void CloseOptionWindow();
	private:
		void LoadingIngameScene();
	};

}

