#pragma once
#include "UIActor.h"
#include "UI_Minimap.h"
#include "UI_OptionWindow.h"
#include "Player.h"

namespace reality
{
	class UI_Actor_Ingame : public UIActor
	{
	public:
		virtual void OnInit(entt::registry& registry);
		virtual void OnUpdate();
	private:
		Player* player_;
	private:
		C_UI* ui_comp_;
	private:
		shared_ptr<UI_Image>	weapon_ui_;
			shared_ptr<UI_Text>		ammo_text_;

		shared_ptr<UI_Minimap>	minimap_ui;

		shared_ptr<UI_Image>	status_ui;
			shared_ptr<UI_Image>	inven_select_;
			shared_ptr<UI_Image>	inven_[4];
			shared_ptr<UI_Text>		inven_slot_text_[4];
			shared_ptr<UI_Text>		inven_count_text_[4];
			shared_ptr<UI_Image>	inven_cooltime_img_[4];
			shared_ptr<UI_Image>	hp_img_;

		shared_ptr<UI_Image>	objective_ui_;
			shared_ptr<UI_Text>		repair_text_;

		shared_ptr<UI_Image>	time_ui_;
			shared_ptr<UI_Text>		time_text_;
			shared_ptr<UI_Text>		kill_text_;

		shared_ptr<UI_Image>	crosshair_ui_;

		shared_ptr<UI_Image>	menu_window_;
			shared_ptr<UI_Image>	menu_title_;
			shared_ptr<UI_Button>	menu_resume_button_;
			shared_ptr<UI_Button>	menu_option_button_;
			shared_ptr<UI_Button>	menu_exit_button_;

		shared_ptr<UI_OptionWindow> menu_option_window_;

		shared_ptr<UI_Image>	interaction_ui_;
			shared_ptr<UI_Image>	interaction_icon_;
			shared_ptr<UI_Image>	interaction_progressbar_;
			shared_ptr<UI_Text>		interaction_text_;
	private:
		float wave_timer_ = 0.0f;
	public:
		void SetGameTimer(float timer);
	private:
		void CreateIngameUI();
		void CreateMenuUI();
	public:
		void UpdateIngameUI();
		void UpdateMenuUI();
	private:
		void OpenMenu();
		void CloseMenu();
		void OpenOptionWindow();
		void CloseOptionWindow();
	};

}

