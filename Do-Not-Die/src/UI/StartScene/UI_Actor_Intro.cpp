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
	//background_->AddChildUI("1_Intro_Logo", intro_logo_);
	intro_logo_->SetLocalRectByMin({ 0, 0 }, win_size_1920_width, win_size_1920_height);
	
	ui_comp.ui_list.insert({ "Intro", intro_logo_ });
	ui_comp.ui_list.insert({ "Background", background_ });

	intro_logo_->SetAlpha(0.0f);
	background_->SetAlpha(1.0f);
}

bool UI_Actor_Intro::UpdateUI()
{
	static float timer = 0.0f;
	if (timer > 11.1f)
		return false;

	timer += TM_DELTATIME;	

	float alpha_1 = FadeAlpha(0.0f, 8.0f, 2.0f, 2.0f, timer);
	intro_logo_->SetAlpha(alpha_1);

	float alpha_2 = FadeAlpha(8.0f, 11.0f, 0.0f, 3.0f, timer);
	background_->SetAlpha(alpha_2);

	if (timer > 8.1f)
	{
		intro_end_ = true;
	}

	return true;
}

