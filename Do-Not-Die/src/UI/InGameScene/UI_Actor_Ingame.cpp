#include "stdafx.h"
#include "UI_Actor_Ingame.h"
#include "SceneMgr.h"
#include "InGameScene.h"

using namespace reality;

void UI_Actor_Ingame::OnInit(entt::registry& registry)
{
	UIActor::OnInit(registry);

	ui_comp_ = &registry.get<C_UI>(GetEntityId());

	CreateIngameUI();

	CreateMenuUI();

}

void UI_Actor_Ingame::OnUpdate()
{
	UIActor::OnUpdate();

	UpdateIngameUI();

	UpdateMenuUI();
}

void UI_Actor_Ingame::CreateIngameUI()
{
	float window_size_width = ENGINE->GetWindowSize().x;
	float window_size_height = ENGINE->GetWindowSize().y;

	// 무기 UI
	weapon_ui_ = make_shared<UI_Image>();
	weapon_ui_->InitImage("T_Handgun_01.png");
	weapon_ui_->SetLocalRectByMin({ 100.0f, window_size_height - 200.0f }, 512.0f, 179.0f);
	ammo_text_ = make_shared<UI_Text>();
	weapon_ui_->AddChildUI(ammo_text_);
	ammo_text_->InitText("30", BASIC, { 320.0f, 72.0f }, 1.5f);
	ui_comp_->ui_list.insert({ "Weapon UI", weapon_ui_ });

	// 상태 + 아이템 UI
	status_ui = make_shared<UI_Image>();
	status_ui->InitImage("T_ItemUI.png");
	status_ui->SetLocalRectByCenter({ window_size_width / 2.0f, window_size_height - 150.0f }, 512.0f, 200.0f);

	hp_img_ = make_shared<UI_Image>();
	status_ui->AddChildUI(hp_img_);
	hp_img_->InitImage("T_HpBar.png");
	hp_img_->SetLocalRectByMin({ status_ui->rect_transform_.world_rect.width / 2.0f - 200.0f, 150.0f }, 400.0f, 30.0f);
	ui_comp_->ui_list.insert({ "Status UI", status_ui });

	// 미니맵 UI
	minimap_ui = make_shared<UI_Minimap>();
	minimap_ui->InitMinimap("T_Minimap.png");
	minimap_ui->SetLocalRectByMin({ window_size_width * 4.0f / 5.0f, window_size_height * 1.0f / 10.0f }, 300.0f, 300.0f);

	auto border_image = make_shared<UI_Image>();
	minimap_ui->AddChildUI(border_image);
	border_image->InitImage("T_Minimap_Border.png");
	border_image->SetLocalRectByMin({ 0,0 }, 300.0f, 300.0f);
	ui_comp_->ui_list.insert({ "Minimap UI", minimap_ui });

	// 시간 UI
	time_ui_ = make_shared<UI_Image>();
	time_ui_->InitImage("T_TimeUI.png");
	time_ui_->SetLocalRectByMin({ 0, 0 }, 300.0f, 100.0f);

	time_text_ = make_shared<UI_Text>();
	time_text_->InitText("10", E_Font::ROTUNDA, { 190.0f, 22.0f });
	time_ui_->AddChildUI(time_text_);
	kill_text_ = make_shared<UI_Text>();
	kill_text_->InitText("300", E_Font::ROTUNDA, { 120.0f, 65.0f });
	time_ui_->AddChildUI(kill_text_);

	ui_comp_->ui_list.insert({ "Time UI", time_ui_ });


	// Objective UI
	objective_ui_ = make_shared<UI_Image>();
	objective_ui_->InitImage("T_ObjectiveUI.png");
	objective_ui_->SetLocalRectByMin({ window_size_width * 4.0f / 5.0f, window_size_height * 4.0f / 10.0f }, 323.0f, 387.0f);

	tire_text_ = make_shared<UI_Text>();
	tire_text_->InitText("TIRE : 10 / 30", E_Font::ROTUNDA, { 50.0f, 140.0f });
	objective_ui_->AddChildUI(tire_text_);

	steel_text_ = make_shared<UI_Text>();
	steel_text_->InitText("STEEL : 10 / 30", E_Font::ROTUNDA, { 50.0f, 200.0f });
	objective_ui_->AddChildUI(steel_text_);

	ui_comp_->ui_list.insert({ "Objective UI", objective_ui_ });

	// CrossHair UI
	crosshair_ui_ = make_shared<UI_Image>();
	crosshair_ui_->InitImage("T_DotCrossHair.png");
	crosshair_ui_->SetLocalRectByCenter({ window_size_width / 2.0f, window_size_height / 2.0f }, 8.0f, 8.0f);
	ui_comp_->ui_list.insert({ "CrossHair UI", crosshair_ui_ });
}

void UI_Actor_Ingame::CreateMenuUI()
{
	float window_size_width = ENGINE->GetWindowSize().x;
	float window_size_height = ENGINE->GetWindowSize().y;

	// Menu Window
	menu_window_ = make_shared<UI_Image>();
	menu_window_->InitImage("T_Opaque.png");
	menu_window_->SetLocalRectByMin({ 0.0f, 0.0f }, window_size_width, window_size_height);

	// Menu Title
	menu_title_ = make_shared<UI_Image>();
	menu_title_->InitImage("T_Ingame_Menu.png");
	menu_window_->AddChildUI(menu_title_);
	menu_title_->SetLocalRectByCenter({ window_size_width / 2.0f, window_size_height / 3.0f }, 897.0f, 141.0f);

	menu_resume_button_;

	// Menu Resume Button
	menu_resume_button_ = make_shared<UI_Button>();
	menu_resume_button_->InitButton("T_Ingame_But_Resume_Normal.png", "T_Ingame_But_Resume_Hover.png");
	menu_window_->AddChildUI(menu_resume_button_);
	menu_resume_button_->SetLocalRectByCenter({ window_size_width / 2.0f, window_size_height / 2.0f + 54.0f * 0.0f }, 269.0f, 54.0f);

	// Menu Option Button
	menu_option_button_ = make_shared<UI_Button>();
	menu_option_button_->InitButton("T_Ingame_But_Option_Normal.png", "T_Ingame_But_Option_Hover.png");
	menu_window_->AddChildUI(menu_option_button_);
	menu_option_button_->SetLocalRectByCenter({ window_size_width / 2.0f, window_size_height / 2.0f + 54.0f * 1.0f}, 269.0f, 54.0f);

	// Menu Option Window
	menu_option_window_ = make_shared<UI_OptionWindow>();
	menu_option_window_->InitOptionWindow();

	// Menu Exit Button
	menu_exit_button_ = make_shared<UI_Button>();
	menu_exit_button_->InitButton("T_Ingame_But_Exit_Normal.png", "T_Ingame_But_Exit_Hover.png");
	menu_window_->AddChildUI(menu_exit_button_);
	menu_exit_button_->SetLocalRectByCenter({ window_size_width / 2.0f, window_size_height / 2.0f + 54.0f * 2.0f }, 269.0f, 54.0f);
}

void UI_Actor_Ingame::UpdateIngameUI()
{
	// HP Update
	player_ = SCENE_MGR->GetPlayer<Player>(0);
	float hp_ratio = (float)player_->GetCurHp() / (float)player_->GetMaxHp();
	if (hp_ratio > 0.3f)
		hp_img_->render_data_.tex_id = "T_HpBar.png";
	else
		hp_img_->render_data_.tex_id = "T_HpBarRed.png";

	hp_img_->SetLocalRectByMin({ status_ui->rect_transform_.world_rect.width / 2.0f - 200.0f, 150.0f }, 400.0f * hp_ratio, 30.0f);

	// Time Update
	static float time = 1000.0f;
	time -= TIMER->GetDeltaTime();
	string time_str = to_string(time);
	time_str = time_str.substr(0, time_str.find('.') + 2 + 1);
	time_text_->SetText(time_str);

	// Kill Update
	static int kill = 0;
	kill = TIMER->game_time;
	kill_text_->SetText(to_string(kill));

	// Vehicle Item Update
	string tire_str = "Tire : ";
	string tire_count = "10";
	string tire_max_count = " / 30";
	tire_text_->SetText(tire_str + tire_count + tire_max_count);
	string steel_str = "Steel : ";
	string steel_count = "10";
	string steel_max_count = " / 30";
	steel_text_->SetText(steel_str + steel_count + steel_max_count);
}

void UI_Actor_Ingame::UpdateMenuUI()
{
	// Menu Open
	static bool menu = false;
	if (DINPUT->GetKeyState(DIK_ESCAPE) == KeyState::KEY_UP)
	{
		menu = !menu;
		if (menu)
			OpenMenu();
		else
			CloseMenu();
	}

	if (!menu)
		return;

	// When Resume Button Selected
	if (menu_resume_button_->GetCurrentState() == E_UIState::UI_SELECT)
	{
		menu_resume_button_->SetCurrentState(E_UIState::UI_NORMAL);
		menu = false;
		CloseMenu();
	}

	// When Resume Button Selected
	if (menu_option_button_->GetCurrentState() == E_UIState::UI_SELECT)
	{
		menu_option_button_->SetCurrentState(E_UIState::UI_NORMAL);
		OpenOptionWindow();
	}

	// If MenuOption Window Opened
	if (ui_comp_->ui_list.find("Menu Option Window") != ui_comp_->ui_list.end())
	{
		if (menu_option_window_->GetCloseButtonState() == E_UIState::UI_SELECT || DINPUT->GetKeyState(DIK_ESCAPE) == KEY_PUSH)
			CloseOptionWindow();
	}


	// When exit Button Selected
	if (menu_exit_button_->GetCurrentState() == E_UIState::UI_SELECT)
	{
		menu_exit_button_->SetCurrentState(E_UIState::UI_NORMAL);
		DestroyWindow(ENGINE->GetWindowHandle());
	}
}

void UI_Actor_Ingame::OpenMenu()
{
	ui_comp_->ui_list.insert({ "Menu UI", menu_window_ });
	
	ui_comp_->ui_list.erase("Weapon UI");
	ui_comp_->ui_list.erase("Minimap UI");
	ui_comp_->ui_list.erase("Status UI");
	ui_comp_->ui_list.erase("Objective UI");
	ui_comp_->ui_list.erase("Time UI");
	ui_comp_->ui_list.erase("Crosshair UI");

	auto game_scene = (InGameScene*)SCENE_MGR->GetScene(INGAME).get();
	game_scene->SetCursorVisible();

}

void UI_Actor_Ingame::CloseMenu()
{
	ui_comp_->ui_list.erase("Menu UI");

	ui_comp_->ui_list.insert({ "Weapon UI", weapon_ui_ });
	ui_comp_->ui_list.insert({ "Minimap UI", minimap_ui });
	ui_comp_->ui_list.insert({ "Status UI", status_ui });
	ui_comp_->ui_list.insert({ "Objective UI", objective_ui_ });
	ui_comp_->ui_list.insert({ "Time UI", time_ui_ });
	ui_comp_->ui_list.insert({ "Crosshair UI", crosshair_ui_ });

	auto game_scene = (InGameScene*)SCENE_MGR->GetScene(INGAME).get();
	game_scene->SetCursorInvisible();
}

void UI_Actor_Ingame::OpenOptionWindow()
{
	//menu_resume_button_->Off();
	//menu_option_button_->Off();
	//menu_exit_button_->Off();

	menu_option_button_->SetCurrentState(E_UIState::UI_NORMAL);

	ui_comp_->ui_list.erase("Menu UI");
	ui_comp_->ui_list.insert({ "Menu Option Window", menu_option_window_ });
}

void UI_Actor_Ingame::CloseOptionWindow()
{
	//menu_resume_button_->On();
	//menu_option_button_->On();
	//menu_exit_button_->On();

	menu_option_window_->SetCloseButtonState(E_UIState::UI_NORMAL);

	ui_comp_->ui_list.erase("Menu Option Window");
	ui_comp_->ui_list.insert({ "Menu UI", menu_window_ });
}

