#include "stdafx.h"
#include "UI_Actor_IntroScene.h"
#include "Engine.h"
#include "SceneMgr.h"

using namespace reality;

void UI_Actor_IntroScene::OnInit(entt::registry& registry)
{
	UIActor::OnInit(registry);

	CreateUI();

}

void UI_Actor_IntroScene::OnUpdate()
{
	UpdateUI();
}

void UI_Actor_IntroScene::CreateUI()
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

void UI_Actor_IntroScene::UpdateUI()
{
	static float alpha = 0.0f;

	if (intro_timer_ < intro_time_)
	{
		intro_timer_ += TIMER->GetDeltaTime();
		if (alpha < 1.0f)
		{
			alpha += TIMER->GetDeltaTime() * 0.2f;
		}
		else
			alpha = 1.0f;
	}
	intro_logo_->SetAlpha(intro_timer_);
}

