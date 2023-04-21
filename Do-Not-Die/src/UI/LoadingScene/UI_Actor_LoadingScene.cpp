#include "stdafx.h"
#include "UI_Actor_LoadingScene.h"
#include "Engine.h"
#include "SceneMgr.h"

using namespace reality;

void UI_Actor_LoadingScene::OnInit(entt::registry& registry)
{
	UIActor::OnInit(registry);

	CreateUI();

}

void UI_Actor_LoadingScene::OnUpdate()
{
	UpdateUI();
}

void UI_Actor_LoadingScene::CreateUI()
{
	C_UI& ui_comp = reg_scene_->get<C_UI>(GetEntityId());

	float win_size_1920_width = E_Resolution_Size[E_Resolution::R1920x1080].x;
	float win_size_1920_height = E_Resolution_Size[E_Resolution::R1920x1080].y;


	// Background UI
	background_ = make_shared<UI_Image>();
	background_->InitImage("T_Background_Loading.png");
	background_->SetLocalRectByMin({ 0, 0 }, win_size_1920_width, win_size_1920_height);

	progress_bar_back_ = make_shared<UI_Image>();
	progress_bar_back_->InitImage("T_Progressbar_Back_Loading.png");
	background_->AddChildUI("1_ProgressBackground", progress_bar_back_);
	progress_bar_back_->SetLocalRectByCenter({ win_size_1920_width / 2.0f, 872.0f }, 1000.0f, 50.0f);
	
	progress_bar_ = make_shared<UI_Image>();
	progress_bar_->InitImage("T_Progressbar_Loading.png");
	progress_bar_back_->AddChildUI("1_ProgressBar", progress_bar_);
	progress_bar_->SetLocalRectByCenter({ progress_bar_back_->rect_transform_[E_Resolution::R1920x1080].world_rect.width / 2.0f,
		progress_bar_back_->rect_transform_[E_Resolution::R1920x1080].world_rect.height / 2.0f }, 950.0f, 30.0f);

	

	ui_comp.ui_list.insert({ "Background", background_ });
}

void UI_Actor_LoadingScene::UpdateUI()
{
}

