#include "stdafx.h"
#include "UI_Actor_StartScene.h"
#include "UI_Button_Newgame.h"
#include "UI_Button_Loadgame.h"
#include "UI_Button_Option.h"
#include "UI_Button_Exit.h"
#include "Engine.h"
#include "SceneMgr.h"

using namespace reality;

void UI_Actor_StartScene::OnInit(entt::registry& registry)
{
	UIActor::OnInit(registry);

	CreateUI();

}

void UI_Actor_StartScene::OnUpdate()
{
	UpdateUI();
}

void UI_Actor_StartScene::CreateUI()
{
	C_UI& ui_comp = reg_scene_->get<C_UI>(GetEntityId());

	float window_size_width = ENGINE->GetWindowSize().x;
	float window_size_height = ENGINE->GetWindowSize().y;

	// Background UI
	background_ = make_shared<UI_Image>();
	background_->InitImage("T_StartScene_background.png");
	background_->SetLocalRectByMin({ 0, 0 }, window_size_width, window_size_height);
	ui_comp.ui_list.insert({ "Background", background_ });
	
	// New Game Button
	newgame_button_ = make_shared<UI_Button_Newgame>();
	newgame_button_->InitButton("T_Button_NewGame_Normal.png", "T_Button_NewGame_Hover.png", "T_Button_NewGame_Pushed.png");
	newgame_button_->SetLocalRectByMin({ window_size_width * 3.0f / 4.0f, window_size_height * 3.0f / 5.0f }, 200.0f, 70.0f);
	ui_comp.ui_list.insert({ "New Game Button", newgame_button_ });

	// Load Game Button
	loadinggame_button = make_shared<UI_Button_Loadgame>();
	loadinggame_button->InitButton("T_Button_LoadGame_Normal.png", "T_Button_LoadGame_Hover.png", "T_Button_LoadGame_Pushed.png");
	loadinggame_button->SetLocalRectByMin({ window_size_width * 3.0f / 4.0f, window_size_height * 3.0f / 5.0f + 70.0f * 1.0f }, 200.0f, 70.0f);
	ui_comp.ui_list.insert({ "Load Game Button", loadinggame_button });

	// Option Button
	option_button_ = make_shared<UI_Button_Option>();
	option_button_->InitButton("T_Button_Option_Normal.png", "T_Button_Option_Hover.png", "T_Button_Option_Pushed.png");
	option_button_->SetLocalRectByMin({ window_size_width * 3.0f / 4.0f, window_size_height * 3.0f / 5.0f + 70.0f * 2.0f }, 200.0f, 70.0f);
	ui_comp.ui_list.insert({ "Option Button", option_button_ });

	// Exit Button
	exit_button_ = make_shared<UI_Button_Exit>();
	exit_button_->InitButton("T_Button_Exit_Normal.png", "T_Button_Exit_Hover.png", "T_Button_Exit_Pushed.png");
	exit_button_->SetLocalRectByMin({ window_size_width * 3.0f / 4.0f, window_size_height * 3.0f / 5.0f + 70.0f * 3.0f }, 200.0f, 70.0f);
	ui_comp.ui_list.insert({ "Exit Button", exit_button_ });
}

void UI_Actor_StartScene::UpdateUI()
{
}

