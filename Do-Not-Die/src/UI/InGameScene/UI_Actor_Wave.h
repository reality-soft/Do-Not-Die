#pragma once
#include "UIActor.h"

namespace reality
{
	class UI_Actor_Wave : public UIActor
	{
	public:
		virtual void OnInit(entt::registry& registry);
		virtual void OnUpdate();

	private:
		C_UI* ui_comp_;
	private:
		shared_ptr<UI_Image>	wave_finish_img_;
		shared_ptr<UI_Image>	wave_start_img_;

	private:
		void CreateUI();
		void UpdateUI();

	public:
		void ShowWaveFinish();
		void ShowWaveStart();
		bool showing_wave_finished_ = false;
		bool showing_wave_started_ = false;
		float fade_timer_ = 0;
	};

}
