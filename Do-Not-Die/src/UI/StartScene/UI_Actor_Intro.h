#pragma once
#include "UIActor.h"

namespace reality
{
	class UI_Actor_Intro : public UIActor
	{
	public:
		virtual void OnInit(entt::registry& registry);
		virtual void OnUpdate();
	private:
		void CreateUI();
		bool UpdateUI();
		float FadeAlpha(float fade_in, float fade_out, float cur_time, float total_time);

	private:
		float intro_time_ = 8.0f;
		float bg_fadeout_time_ = 3.0f;

	private:
		shared_ptr<UI_Image>	background_;
		shared_ptr<UI_Image>	intro_logo_;
		//shared_ptr<UI_Image>	progress_bar_;
	public:
		void	SetIntroTime(float time) { intro_time_ = time; }
		float	GetIntroTime() { return intro_time_; }
		bool intro_end_ = false;		
	};

}

