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
	background_->InitImage("T_StartScene_background.png");
	background_->SetLocalRectByMin({ 0, 0 }, window_size_width, window_size_height);
	ui_comp.ui_list.insert({ "Background", background_ });
}

void UI_Actor_LoadingScene::UpdateUI()
{
}

