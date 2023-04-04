#include "stdafx.h"
#include "UI_StartScene_Actor.h"
#include "UI_Newgame_Button.h"
#include "SceneMgr.h"

using namespace reality;

void UI_StartScene_Actor::OnInit(entt::registry& registry)
{
	UIActor::OnInit(registry);

	CreateUI();

}

void UI_StartScene_Actor::OnUpdate()
{
	UpdateUI();
}

void UI_StartScene_Actor::CreateUI()
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
	newgame_button_ = make_shared<UI_Newgame_Button>();
	newgame_button_->InitButton("T_Button_NewGame_Normal.png", "T_Button_NewGame_Hover.png", "T_Button_NewGame_Pushed.png");
	newgame_button_->SetLocalRectByMin({ window_size_width * 3.0f / 4.0f, window_size_height * 3.0f / 4.0f }, 200.0f, 70.0f);
	ui_comp.ui_list.insert({ "New Game Button", newgame_button_ });
}

void UI_StartScene_Actor::UpdateUI()
{
}

