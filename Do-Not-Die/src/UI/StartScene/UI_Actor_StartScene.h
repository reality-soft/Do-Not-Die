#pragma once
#include "UIActor.h"
#include "UI_OptionWindow.h"

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
		shared_ptr<UI_Image>	title_img_;
		shared_ptr<UI_Button>	newgame_button_;
		shared_ptr<UI_Button>	loadinggame_button;
		shared_ptr<UI_Button>	option_button_;
		shared_ptr<UI_Button>	exit_button_;

		shared_ptr<UI_OptionWindow>	option_window_;
	private:
		void CreateUI();
		void UpdateUI();
	private:
		void OpenOptionWindow();
		void CloseOptionWindow();
	};

}

