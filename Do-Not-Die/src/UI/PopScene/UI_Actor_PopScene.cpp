#include "stdafx.h"
#include "UI_Actor_PopScene.h"
#include "SceneMgr.h"
#include "StartScene.h"

using namespace reality;

void UI_Actor_PopScene::OnInit(entt::registry& registry)
{
	UIActor::OnInit(registry);

	CreateUI();
}

void UI_Actor_PopScene::OnUpdate()
{
	UpdateUI();
}

void UI_Actor_PopScene::CreateUI()
{
	C_UI& ui_comp = reg_scene_->get<C_UI>(GetEntityId());

	float screen_width = ENGINE->GetWindowSize().x;
	float screen_height = ENGINE->GetWindowSize().y;

	// Background UI
	background_ = make_shared<UI_Image>();
	background_->InitImage("T_Background_Pop.png");
	background_->rect_transform_[RPOP].local_rect.SetRectByMin({ 0.0f, 0.0f }, screen_width, screen_height);
	ui_comp.ui_list.insert({ "1_Background", background_ });
	background_->UpdateRectTransform();

	// Button UI
	start_button_ = make_shared<UI_Button>();
	start_button_->InitButton("T_PopScene_Start_Btn_Normal.png", "T_PopScene_Start_Btn_Hover.png", "T_PopScene_Start_Btn_Pushed.png");
	start_button_->rect_transform_[RPOP].local_rect.SetRectByCenter({ screen_width / 2.0f - 70.0f , screen_height * 5.0f / 6.0f}, 120.0f, 50.0f);
	start_button_->UpdateRectTransform();
	ui_comp.ui_list.insert({ "2_Start Button", start_button_ });

	exit_button_ = make_shared<UI_Button>();
	exit_button_->InitButton("T_PopScene_Exit_Btn_Normal.png", "T_PopScene_Exit_Btn_Hover.png", "T_PopScene_Exit_Btn_Pushed.png");
	exit_button_->rect_transform_[RPOP].local_rect.SetRectByCenter({ screen_width / 2.0f + 70.0f , screen_height * 5.0f / 6.0f }, 120.0f, 50.0f);
	exit_button_->UpdateRectTransform();
	ui_comp.ui_list.insert({ "2_Exit Button", exit_button_ });

	// Loading Progress Text
	progress_text_ = make_shared<UI_Text>();
	progress_text_->InitText("TEST", BASIC, {0, screen_height - 50.0f}, 0.5f);
	progress_text_->rect_transform_[RPOP].local_rect.SetRectByMin({ 0, screen_height - 20.0f }, 10.0f, 10.0f);
	progress_text_->UpdateRectTransform();
	ui_comp.ui_list.insert({ "2_Progress Text", progress_text_ });
}

void UI_Actor_PopScene::UpdateUI()
{
	
}

