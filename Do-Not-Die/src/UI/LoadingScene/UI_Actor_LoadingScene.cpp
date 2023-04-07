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

	float window_size_width = ENGINE->GetWindowSize().x;
	float window_size_height = ENGINE->GetWindowSize().y;

	// Background UI
	background_ = make_shared<UI_Image>();
	background_->InitImage("T_Background_Loading.png");
	background_->SetLocalRectByMin({ 0, 0 }, window_size_width, window_size_height);

	progress_bar_back_ = make_shared<UI_Image>();
	progress_bar_back_->InitImage("T_Progressbar_Back_Loading.png");
	background_->AddChildUI(progress_bar_back_);
	progress_bar_back_->SetLocalRectByCenter({ window_size_width / 2.0f, 872.0f }, 1000.0f, 50.0f);
	
	progress_bar_ = make_shared<UI_Image>();
	progress_bar_->InitImage("T_Progressbar_Loading.png");
	progress_bar_back_->AddChildUI(progress_bar_);
	progress_bar_->SetLocalRectByCenter({ progress_bar_back_->rect_transform_.world_rect.width / 2.0f, progress_bar_back_->rect_transform_.world_rect.height / 2.0f }, 950.0f, 30.0f);

	

	ui_comp.ui_list.insert({ "Background", background_ });
}

void UI_Actor_LoadingScene::UpdateUI()
{
}

