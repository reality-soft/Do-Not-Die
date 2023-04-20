#include "stdafx.h"
#include "UI_Actor_Ingame.h"
#include "SceneMgr.h"
#include "InGameScene.h"
#include "ItemBase.h"

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

void reality::UI_Actor_Ingame::SetGameTimer(float timer)
{
	wave_timer_ = timer;
}

void UI_Actor_Ingame::CreateIngameUI()
{
	float win_size_1920_width = E_Resolution_Size[E_Resolution::R1920x1080].x;
	float win_size_1920_height = E_Resolution_Size[E_Resolution::R1920x1080].y;

	// 무기 UI
	weapon_ui_ = make_shared<UI_Image>();
	weapon_ui_->InitImage("T_Handgun_01.png");
	weapon_ui_->SetLocalRectByMin({ 100.0f, win_size_1920_height - 200.0f }, 512.0f, 179.0f);
	ammo_text_ = make_shared<UI_Text>();
	weapon_ui_->AddChildUI(ammo_text_);
	ammo_text_->InitText("30", BASIC, { 320.0f, 65.0f }, 1.3f);
	ui_comp_->ui_list.insert({ "Weapon UI", weapon_ui_ });

	// 상태 + 아이템 UI
	status_ui = make_shared<UI_Image>();
	status_ui->InitImage("T_ItemUI.png");
	status_ui->SetLocalRectByCenter({ win_size_1920_width / 2.0f, win_size_1920_height - 150.0f }, 512.0f, 200.0f);

	inven_[0] = make_shared<UI_Image>();
	inven_[0]->InitImage("");
	inven_[0]->SetLocalRectByCenter({ 101.5f, 76.5f }, 90.0f, 90.0f);
	inven_text_[0] = make_shared<UI_Text>();
	inven_text_[0]->InitText("", E_Font::ROTUNDA, { 70.0f, 70.0f }, 0.8f);
	inven_[0]->AddChildUI(inven_text_[0]);
	inven_cooltime_img_[0] = make_shared<UI_Image>();
	inven_cooltime_img_[0]->InitImage("T_ItemCoolTime.png");
	inven_[0]->AddChildUI(inven_cooltime_img_[0]);
	inven_cooltime_img_[0]->SetLocalRectByMin({0.0f, 0.0f}, 90.0f, 90.0f);

	inven_[1] = make_shared<UI_Image>();
	inven_[1]->InitImage("");
	inven_[1]->SetLocalRectByCenter({ 203.5f, 76.5f }, 90.0f, 90.0f);
	inven_text_[1] = make_shared<UI_Text>();
	inven_text_[1]->InitText("", E_Font::ROTUNDA, { 70.0f, 70.0f }, 0.8f);
	inven_[1]->AddChildUI(inven_text_[1]);
	inven_cooltime_img_[1] = make_shared<UI_Image>();
	inven_cooltime_img_[1]->InitImage("T_ItemCoolTime.png");
	inven_[1]->AddChildUI(inven_cooltime_img_[1]);
	inven_cooltime_img_[1]->SetLocalRectByMin({ 0.0f, 0.0f }, 90.0f, 90.0f);

	inven_[2] = make_shared<UI_Image>();
	inven_[2]->InitImage("");
	inven_[2]->SetLocalRectByCenter({ 306.5f, 76.5f }, 90.0f, 90.0f);
	inven_text_[2] = make_shared<UI_Text>();
	inven_text_[2]->InitText("", E_Font::ROTUNDA, { 70.0f, 70.0f }, 0.8f);
	inven_[2]->AddChildUI(inven_text_[2]);
	inven_cooltime_img_[2] = make_shared<UI_Image>();
	inven_cooltime_img_[2]->InitImage("T_ItemCoolTime.png");
	inven_[2]->AddChildUI(inven_cooltime_img_[2]);
	inven_cooltime_img_[2]->SetLocalRectByMin({ 0.0f, 0.0f }, 90.0f, 90.0f);

	inven_[3] = make_shared<UI_Image>();
	inven_[3]->InitImage("");
	inven_[3]->SetLocalRectByCenter({ 410.5f, 76.5f }, 90.0f, 90.0f);
	inven_text_[3] = make_shared<UI_Text>();
	inven_text_[3]->InitText("", E_Font::ROTUNDA, { 70.0f, 70.0f }, 0.8f);
	inven_[3]->AddChildUI(inven_text_[3]);
	inven_cooltime_img_[3] = make_shared<UI_Image>();
	inven_cooltime_img_[3]->InitImage("T_ItemCoolTime.png");
	inven_[3]->AddChildUI(inven_cooltime_img_[3]);
	inven_cooltime_img_[3]->SetLocalRectByMin({ 0.0f, 0.0f }, 90.0f, 90.0f);

	hp_img_ = make_shared<UI_Image>();
	status_ui->AddChildUI(hp_img_);
	hp_img_->InitImage("T_HpBar.png");
	hp_img_->SetLocalRectByMin({ status_ui->rect_transform_[E_Resolution::R1920x1080].world_rect.width / 2.0f - 200.0f, 150.0f}, 400.0f, 30.0f);
	ui_comp_->ui_list.insert({ "Status UI", status_ui });

	// 미니맵 UI
	minimap_ui = make_shared<UI_Minimap>();
	minimap_ui->InitMinimap("T_Minimap.png");
	minimap_ui->SetLocalRectByMin({ win_size_1920_width * 4.0f / 5.0f, win_size_1920_height * 1.0f / 10.0f }, 300.0f, 300.0f);

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
	objective_ui_->SetLocalRectByMin({ win_size_1920_width * 4.0f / 5.0f, win_size_1920_height * 4.0f / 10.0f }, 323.0f, 387.0f);

	repair_text_ = make_shared<UI_Text>();
	repair_text_->InitText("Repair Count : 0 / 5", E_Font::BASIC, { 50.0f, 140.0f }, 0.7f);
	objective_ui_->AddChildUI(repair_text_);


	ui_comp_->ui_list.insert({ "Objective UI", objective_ui_ });

	// CrossHair UI
	crosshair_ui_ = make_shared<UI_Image>();
	crosshair_ui_->InitImage("T_DotCrossHair.png");
	crosshair_ui_->SetLocalRectByCenter({ win_size_1920_width / 2.0f, win_size_1920_height / 2.0f }, 8.0f, 8.0f);
	ui_comp_->ui_list.insert({ "CrossHair UI", crosshair_ui_ });

	// Interaction UI
	interaction_ui_ = make_shared<UI_Image>();
	interaction_ui_->InitImage("T_Interaction.png");
	interaction_ui_->SetLocalRectByMin({win_size_1920_width * 3.0f / 5.0f, win_size_1920_height / 2.0f }, 200.0f, 40.0f);

	interaction_progressbar_ = make_shared<UI_Image>();
	interaction_progressbar_->InitImage("T_Interaction_Progress.png");
	interaction_progressbar_->SetLocalRectByMin({ 0.0f, 0.0f }, 200.0f, 40.0f);

	interaction_icon_ = make_shared<UI_Image>();
	interaction_icon_->InitImage("T_E_Icon.png");
	interaction_icon_->SetLocalRectByMin({ 2.5f, 2.5f }, 35.0f, 35.0f);
	interaction_ui_->AddChildUI(interaction_icon_);

	interaction_text_ = make_shared<UI_Text>();
	interaction_text_->InitText("", E_Font::BASIC, { 40.0f, 8.0f }, 0.5f);
	interaction_ui_->AddChildUI(interaction_text_);
}

void UI_Actor_Ingame::CreateMenuUI()
{
	float win_size_1920_width = E_Resolution_Size[E_Resolution::R1920x1080].x;
	float win_size_1920_height = E_Resolution_Size[E_Resolution::R1920x1080].y;

	// Menu Window
	menu_window_ = make_shared<UI_Image>();
	menu_window_->InitImage("T_Opaque.png");
	menu_window_->SetLocalRectByMin({ 0.0f, 0.0f }, win_size_1920_width, win_size_1920_height);

	// Menu Title
	menu_title_ = make_shared<UI_Image>();
	menu_title_->InitImage("T_Ingame_Menu.png");
	menu_window_->AddChildUI(menu_title_);
	menu_title_->SetLocalRectByCenter({ win_size_1920_width / 2.0f, win_size_1920_height / 3.0f }, 897.0f, 141.0f);

	menu_resume_button_;

	// Menu Resume Button
	menu_resume_button_ = make_shared<UI_Button>();
	menu_resume_button_->InitButton("T_Ingame_But_Resume_Normal.png", "T_Ingame_But_Resume_Hover.png");
	menu_window_->AddChildUI(menu_resume_button_);
	menu_resume_button_->SetLocalRectByCenter({ win_size_1920_width / 2.0f, win_size_1920_height / 2.0f + 54.0f * 0.0f }, 269.0f, 54.0f);

	// Menu Option Button
	menu_option_button_ = make_shared<UI_Button>();
	menu_option_button_->InitButton("T_Ingame_But_Option_Normal.png", "T_Ingame_But_Option_Hover.png");
	menu_window_->AddChildUI(menu_option_button_);
	menu_option_button_->SetLocalRectByCenter({ win_size_1920_width / 2.0f, win_size_1920_height / 2.0f + 54.0f * 1.0f}, 269.0f, 54.0f);

	// Menu Option Window
	menu_option_window_ = make_shared<UI_OptionWindow>();
	menu_option_window_->InitOptionWindow();

	// Menu Exit Button
	menu_exit_button_ = make_shared<UI_Button>();
	menu_exit_button_->InitButton("T_Ingame_But_Exit_Normal.png", "T_Ingame_But_Exit_Hover.png");
	menu_window_->AddChildUI(menu_exit_button_);
	menu_exit_button_->SetLocalRectByCenter({ win_size_1920_width / 2.0f, win_size_1920_height / 2.0f + 54.0f * 2.0f }, 269.0f, 54.0f);
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

	hp_img_->SetLocalRectByMin({ status_ui->rect_transform_[E_Resolution::R1920x1080].world_rect.width / 2.0f - 200.0f, 150.0f}, 400.0f * hp_ratio, 30.0f);

	// Time Update
	string time_str = to_string(wave_timer_);
	time_str = time_str.substr(0, time_str.find('.') + 2 + 1);
	time_text_->SetText(time_str);

	// Kill Update
	static int kill = 0;
	kill = TIMER->game_time;
	kill_text_->SetText(to_string(kill));

	// Vehicle Item Update
	string repair_str = "Repair Count : ";
	auto ingame_scene = (InGameScene*)SCENE_MGR->GetScene(INGAME).get();
	auto wave_sys = ingame_scene->GetWaveSystem();
	string repair_count = to_string(wave_sys.car_repaired);
	string repair_max_count = " / 5";
	repair_text_->SetText(repair_str + repair_count + repair_max_count);

	// Cross Hair Update
	if (ui_comp_->ui_list.find("CrossHair UI") != ui_comp_->ui_list.end())
	{
		if(!player_->IsAiming())
			ui_comp_->ui_list.erase("CrossHair UI");
	}
	else
	{
		if (player_->IsAiming())
			ui_comp_->ui_list.insert(make_pair("CrossHair UI", crosshair_ui_));
	}

	// Item UI Update
	if (player_ != nullptr)
	{
		auto& inventory = player_->GetInventory();
		auto& inventory_timer = player_->GetInventoryTimer();
		for (int i = 0; i < INVENTORY_MAX; i++)
		{
			if (inventory[i] == NULL)
			{
				status_ui->DeleteChildUI(inven_[i]);
			}
			else
			{
				status_ui->AddChildUI(inven_[i]);
				inven_[i]->SetImage(inventory[i]->GetIcon());
				inven_text_[i]->SetText(to_string(inventory[i]->GetCount()));

				auto& cooltime_local_rect = inven_cooltime_img_[i]->rect_transform_[E_Resolution::R1920x1080].local_rect;
				float percentage = (inventory[i]->GetCooltime() - inventory_timer[i]) / inventory[i]->GetCooltime();
				static float first_height = inven_[i]->rect_transform_[E_Resolution::R1920x1080].local_rect.height;
				inven_cooltime_img_[i]->SetLocalRectByMax(cooltime_local_rect.max, cooltime_local_rect.width, 
					first_height * percentage);
			}
		}
	}
	
	// Interaction UI Update
	if (player_ != nullptr)
	{
		if (player_->selectable_counts_ != 0 || player_->can_extract_repair_ || player_->can_repair_car_)
		{
			if(ui_comp_->ui_list.find("Interaction UI") == ui_comp_->ui_list.end())
				ui_comp_->ui_list.insert({ "Interaction UI", interaction_ui_ });

			// Item Interaction
			if (player_->selectable_counts_ != 0)
			{
				string item = "";
				switch (player_->selectable_items_.begin()->second->item_type_)
				{
				case ItemType::eHealKit:
					item = "Heal Kit";
					break;
				case ItemType::eMedicalBox:
					item = "Medical Box";
					break;
				case ItemType::eEnergyDrink:
					item = "Energy Drink";
					break;
				case ItemType::eDrug:
					item = "Drug";
					break;
				case ItemType::eAR_Ammo:
					item = "AR Ammo";
					break;
				case ItemType::ePistol_Ammo:
					item = "Pistol Ammo";
					break;
				case ItemType::eGrenade:
					item = "Grenade";
					break;
				case ItemType::eRepairPart:
					item = "Repair Part";
					break;
				}
				interaction_text_->SetText("Get " + item);
				interaction_ui_->DeleteChildUI(interaction_progressbar_);

			}
			// Extract Car Interaction
			if (player_->can_extract_repair_)
			{
				interaction_text_->SetText("Push to Extract");
				interaction_ui_->AddChildUI(interaction_progressbar_);
				float width = 200.0f * player_->extract_during_time_ / player_->extract_time_takes_;
				interaction_progressbar_->SetLocalRectByMin({ 0.0f, 0.0f }, width, 40.0f);
			}
			// Repair Car Interaction
			if (player_->can_repair_car_)
			{
				interaction_text_->SetText("Push to Repair");
				interaction_ui_->AddChildUI(interaction_progressbar_);
				float width = 200.0f * player_->repair_during_time_ / player_->repair_time_takes_;
				interaction_progressbar_->SetLocalRectByMin({ 0.0f, 0.0f }, width, 40.0f);
			}
		}
		else if((player_->selectable_counts_ == 0 && !player_->can_extract_repair_) && ui_comp_->ui_list.find("Interaction UI") != ui_comp_->ui_list.end())
			ui_comp_->ui_list.erase("Interaction UI");
	}
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

	if (menu_option_window_->resolution_value != menu_option_window_->option_resolution_list_box_->GetCurrentItem())
	{
		menu_option_window_->resolution_value = menu_option_window_->option_resolution_list_box_->GetCurrentItem();
		ENGINE->Resize((E_Resolution)menu_option_window_->option_resolution_list_box_->GetCurrentIndex());
		CloseOptionWindow();
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
	ui_comp_->ui_list.erase("CrossHair UI");

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
	ui_comp_->ui_list.insert({ "CrossHair UI", crosshair_ui_ });

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

