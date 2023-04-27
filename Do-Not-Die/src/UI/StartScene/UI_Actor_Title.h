#pragma once
#include "UIActor.h"
#include "UI_OptionWindow.h"

namespace reality
{
	class UI_Actor_Title : public UIActor
	{
	public:
		virtual void OnInit(entt::registry& registry);
		virtual void OnUpdate();

		void FadeInUI();
		void DisappearUI();

	private:
		C_UI* ui_comp_;
	private: 
		shared_ptr<UI_Image>	title_img_;
		shared_ptr<UI_Image>	background_img_;
		shared_ptr<UI_Image>	direct_x_img_;
		shared_ptr<UI_Image>	fmod_img_;
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

