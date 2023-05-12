#include "stdafx.h"
#include "UI_Actor_Wave.h"
#include "SceneMgr.h"
#include "InGameScene.h"

using namespace reality;

void UI_Actor_Wave::OnInit(entt::registry& registry)
{
	UIActor::OnInit(registry);

	CreateUI();

}

void UI_Actor_Wave::OnUpdate()
{
	UpdateUI();
}

void UI_Actor_Wave::CreateUI()
{
	ui_comp_ = &reg_scene_->get<C_UI>(GetEntityId());

	float win_size_1920_width = E_Resolution_Size[E_Resolution::R1920x1080].x;
	float win_size_1920_height = E_Resolution_Size[E_Resolution::R1920x1080].y;


	// Background UI
	wave_finish_img_ = make_shared<UI_Image>();
	wave_finish_img_->InitImage("T_DayHasDawned.png");
	wave_finish_img_->SetLocalRectByMin({ 0, 0 }, win_size_1920_width, win_size_1920_height);

	wave_start_img_ = make_shared<UI_Image>();
	wave_start_img_->InitImage("T_NightHasCome.png");
	wave_start_img_->SetLocalRectByMin({ 0, 0 }, win_size_1920_width, win_size_1920_height);
}

void UI_Actor_Wave::UpdateUI()
{
	if (showing_wave_finished_)
	{
		float alpha = 0.0f;
		fade_timer_ += TM_DELTATIME;

		if (fade_timer_ > 7.0f)
			showing_wave_finished_ = false;

		else if (fade_timer_ > 5.0f)
			alpha = FadeOutAlpha(5.0f, 7.0f, fade_timer_);

		else
			alpha = FadeInAlpha(0.0f, 2.0f, fade_timer_);

		wave_finish_img_->SetAlpha(alpha);
	}
	else if (showing_wave_started_)
	{
		float alpha = 0.0f;
		fade_timer_ += TM_DELTATIME;

		if (fade_timer_ > 7.0f)
			showing_wave_started_ = false;

		else if (fade_timer_ > 5.0f)
			alpha = FadeOutAlpha(5.0f, 7.0f, fade_timer_);

		else
			alpha = FadeInAlpha(0.0f, 2.0f, fade_timer_);

		wave_start_img_->SetAlpha(alpha);
	}
	else
	{
		ui_comp_->ui_list.clear();
		fade_timer_ = 0.0f;
	}
}

void UI_Actor_Wave::ShowWaveFinish()
{
	showing_wave_finished_ = true;
	ui_comp_->ui_list.insert({ "Wave Finish Image", wave_finish_img_ });
	wave_finish_img_->SetAlpha(0.0f);
}
void UI_Actor_Wave::ShowWaveStart()
{
	showing_wave_started_ = true;
	ui_comp_->ui_list.insert({ "Wave Start Image", wave_start_img_ });
	wave_start_img_->SetAlpha(0.0f);
}