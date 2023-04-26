#include "stdafx.h"
#include "UI_Actor_Intro.h"
#include "Engine.h"
#include "SceneMgr.h"

using namespace reality;

void UI_Actor_Intro::OnInit(entt::registry& registry)
{
	UIActor::OnInit(registry);

	CreateUI();

}

void UI_Actor_Intro::OnUpdate()
{
	if (UpdateUI() == false)
	{
		C_UI& ui_comp = reg_scene_->get<C_UI>(GetEntityId());
		ui_comp.ui_list.clear();
	}
}

void UI_Actor_Intro::CreateUI()
{
	C_UI& ui_comp = reg_scene_->get<C_UI>(GetEntityId());

	float win_size_1920_width = E_Resolution_Size[E_Resolution::R1920x1080].x;
	float win_size_1920_height = E_Resolution_Size[E_Resolution::R1920x1080].y;


	// Background UI
	background_ = make_shared<UI_Image>();
	background_->InitImage("T_Intro_Background.png");
	background_->SetLocalRectByMin({ 0, 0 }, win_size_1920_width, win_size_1920_height);

	intro_logo_ = make_shared<UI_Image>();
	intro_logo_->InitImage("T_IntroLogo.png");
	background_->AddChildUI("1_Intro_Logo", intro_logo_);
	intro_logo_->SetLocalRectByMin({ 0, 0 }, win_size_1920_width, win_size_1920_height);
	
	ui_comp.ui_list.insert({ "Background", background_ });
}

bool UI_Actor_Intro::UpdateUI()
{
	static float timer = 0.0f;
	timer += TM_DELTATIME;

	if (timer < intro_time_)
	{
		intro_logo_->SetAlpha(FadeAlpha(2.0f, 2.0f, timer, intro_time_));
	}
	else if (timer < intro_time_ + bg_fadeout_time_)
	{
		intro_end_ = true;
		background_->SetAlpha(FadeAlpha(0.0f, 2.0f, timer, intro_time_ + bg_fadeout_time_));
	}
	else
	{
		return false;
	}

	return true;
}

float reality::UI_Actor_Intro::FadeAlpha(float fade_in, float fade_out, float cur_time, float total_time)
{
	float alpha = 1.0f;

	float fi_s, fi_e, fo_s, fo_e;
	fi_s = 0.0f;
	fi_e = fi_s + fade_in;
	fo_s = total_time - fade_out;
	fo_e = total_time;

	if (fi_s <= cur_time && cur_time <= fi_e) // fade_in
	{
		alpha = cur_time / fade_in;
	}
	else if (fo_s <= cur_time && cur_time <= fo_e) // fade_out
	{
		alpha = 1.0f - (cur_time - fo_s) / fade_out;
	}

	return alpha;
}


