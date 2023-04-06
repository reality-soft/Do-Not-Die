#include "stdafx.h"
#include "UI_Actor_StartScene.h"
#include "SceneMgr.h"
#include "LoadingScene.h"
#include "InGameScene.h"

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
	ui_comp_ = &reg_scene_->get<C_UI>(GetEntityId());

	float window_size_width = ENGINE->GetWindowSize().x;
	float window_size_height = ENGINE->GetWindowSize().y;

	// Background UI
	background_ = make_shared<UI_Image>();
	background_->InitImage("T_StartScene_background.png");
	background_->SetLocalRectByMin({ 0, 0 }, window_size_width, window_size_height);
	ui_comp_->ui_list.insert({ "Background", background_ });
	
	// New Game Button
	newgame_button_ = make_shared<UI_Button>();
	newgame_button_->InitButton("T_Button_NewGame_Normal.png", "T_Button_NewGame_Hover.png", "T_Button_NewGame_Pushed.png");
	newgame_button_->SetLocalRectByMin({ window_size_width * 3.0f / 4.0f, window_size_height * 3.0f / 5.0f }, 200.0f, 70.0f);
	ui_comp_->ui_list.insert({ "New Game Button", newgame_button_ });

	// Load Game Button
	loadinggame_button = make_shared<UI_Button>();
	loadinggame_button->InitButton("T_Button_LoadGame_Normal.png", "T_Button_LoadGame_Hover.png", "T_Button_LoadGame_Pushed.png");
	loadinggame_button->SetLocalRectByMin({ window_size_width * 3.0f / 4.0f, window_size_height * 3.0f / 5.0f + 70.0f * 1.0f }, 200.0f, 70.0f);
	ui_comp_->ui_list.insert({ "Load Game Button", loadinggame_button });

	// Option Button
	option_button_ = make_shared<UI_Button>();
	option_button_->InitButton("T_Button_Option_Normal.png", "T_Button_Option_Hover.png", "T_Button_Option_Pushed.png");
	option_button_->SetLocalRectByMin({ window_size_width * 3.0f / 4.0f, window_size_height * 3.0f / 5.0f + 70.0f * 2.0f }, 200.0f, 70.0f);
	ui_comp_->ui_list.insert({ "Option Button", option_button_ });

	// Exit Button
	exit_button_ = make_shared<UI_Button>();
	exit_button_->InitButton("T_Button_Exit_Normal.png", "T_Button_Exit_Hover.png", "T_Button_Exit_Pushed.png");
	exit_button_->SetLocalRectByMin({ window_size_width * 3.0f / 4.0f, window_size_height * 3.0f / 5.0f + 70.0f * 3.0f }, 200.0f, 70.0f);
	ui_comp_->ui_list.insert({ "Exit Button", exit_button_ });

	// Opaque
	option_opaque_ = make_shared<UI_Image>();
	option_opaque_->InitImage("T_Opaque.png");
	option_opaque_->SetLocalRectByMin({ 0.0f, 0.0f }, window_size_width, window_size_height);

	// Option Window
	option_window_ = make_shared<UI_Image>();
	option_window_->InitImage("T_Option.png");
	option_opaque_->AddChildUI(option_window_);
	option_window_->SetLocalRectByCenter({ window_size_width / 2.0f, window_size_height / 2.0f }, 246.0f * 2.0f, 305.0f * 2.0f);

	// Option Close Button
	option_close_button_ = make_shared<UI_Button>();
	option_close_button_->InitButton("T_Button_Close_Normal.png", "T_Button_Close_Hover.png", "T_Button_Close_Normal.png");
	option_window_->AddChildUI(option_close_button_);
	option_close_button_->SetLocalRectByCenter({ 370.0f, 80.0f }, 51.0f, 56.0f);

}

void UI_Actor_StartScene::UpdateUI()
{
	// When NewGame Button Selected
	if (newgame_button_->GetCurrentState() == E_UIState::UI_SELECT)
	{
		thread loading_thread = thread(&UI_Actor_StartScene::LoadingIngameScene, this);
		loading_thread.detach();
		SCENE_MGR->AddScene<LoadingScene>(E_SceneType::LOADING);
		SCENE_MGR->ChangeScene(E_SceneType::LOADING);
	}

	// When exit Button Selected
	if (exit_button_->GetCurrentState() == E_UIState::UI_SELECT)
	{
		DestroyWindow(ENGINE->GetWindowHandle());
	}

	// When Option Button Selected
	if (option_button_->GetCurrentState() == E_UIState::UI_SELECT)
		OpenOptionWindow();

	// If Option Window Opened
	if (ui_comp_->ui_list.find("Option Window") != ui_comp_->ui_list.end())
	{
		if (option_close_button_->GetCurrentState() == E_UIState::UI_SELECT || DINPUT->GetKeyState(DIK_ESCAPE) == KEY_PUSH)
			CloseOptionWindow();
	}
}

void UI_Actor_StartScene::OpenOptionWindow()
{
	newgame_button_->Off();
	loadinggame_button->Off();
	option_button_->Off();
	exit_button_->Off();

	option_button_->SetCurrentState(E_UIState::UI_NORMAL);

	ui_comp_->ui_list.insert({ "Option Window", option_opaque_ });
}

void UI_Actor_StartScene::CloseOptionWindow()
{
	newgame_button_->On();
	loadinggame_button->On();
	option_button_->On();
	exit_button_->On();

	option_close_button_->SetCurrentState(E_UIState::UI_NORMAL);

	ui_comp_->ui_list.erase("Option Window");
}


void UI_Actor_StartScene::LoadingIngameScene()
{
	SCENE_MGR->AddScene<InGameScene>(INGAME);
}