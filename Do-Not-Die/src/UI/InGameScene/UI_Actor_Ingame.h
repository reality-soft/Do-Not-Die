#pragma once
#include "UIActor.h"
#include "UI_Minimap.h"
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
		shared_ptr<UI_Image>	weapon_ui_;
			shared_ptr<UI_Text>		ammo_text_;
		shared_ptr<UI_Minimap>	minimap_ui;
		shared_ptr<UI_Image>	status_ui;
		shared_ptr<UI_Image>	objective_ui_;
			shared_ptr<UI_Text>		tire_text_;
			shared_ptr<UI_Text>		steel_text_;
		shared_ptr<UI_Image>	time_ui_;
			shared_ptr<UI_Text>		time_text_;
			shared_ptr<UI_Text>		kill_text_;
		shared_ptr<UI_Image>	crosshair_ui_;
		// Need to be Updated UI
	private:
		shared_ptr<UI_Image>	hp_img_;
	private:
		void CreateUI();
		void UpdateUI();
	};

}

