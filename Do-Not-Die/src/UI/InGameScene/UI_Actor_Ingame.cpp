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

void UI_Actor_Ingame::SetGameTimer(float timer)
{
	wave_timer_ = timer;
}

void UI_Actor_Ingame::CreateIngameUI()
{
	float win_size_1920_width = E_Resolution_Size[E_Resolution::R1920x1080].x;
	float win_size_1920_height = E_Resolution_Size[E_Resolution::R1920x1080].y;

	// 무기 UI
	{
		float weapon_ui_height = win_size_1920_height - 500.0f;
		
		float width = 300.0f;
		float height = 75.0f;

		float height_step = 78.0f;

		XMFLOAT2 text_pos = { 10.0f, 20.0f };
		float text_size = 0.75f;
		// AR
		ar_ui_ = make_shared<UI_Image>();
		ar_ui_->InitImage("T_Weapon_AR.png");
		ar_ui_->SetLocalRectByMin({ 100.0f, weapon_ui_height + height_step * 0.0f }, width, height);
		ar_ammo_text_ = make_shared<UI_Text>();
		ar_ui_->AddChildUI("1_AmmoText", ar_ammo_text_);
		ar_ammo_text_->InitText("30", ROBOTO, text_pos, text_size);
		ui_comp_->ui_list.insert({ "Weapon_AR UI", ar_ui_ });

		// Handgun
		handgun_ui_ = make_shared<UI_Image>();
		handgun_ui_->InitImage("T_Weapon_Handgun.png");
		handgun_ui_->SetLocalRectByMin({ 100.0f, weapon_ui_height + height_step * 1.0f }, width, height);
		handgun_ammo_text_ = make_shared<UI_Text>();
		handgun_ui_->AddChildUI("1_AmmoText", handgun_ammo_text_);
		handgun_ammo_text_->InitText("30", ROBOTO, text_pos, text_size);
		ui_comp_->ui_list.insert({ "Weapon_Handgun UI", handgun_ui_ });

		// Meele
		meele_ui_ = make_shared<UI_Image>();
		meele_ui_->InitImage("T_Weapon_Meele.png");
		meele_ui_->SetLocalRectByMin({ 100.0f, weapon_ui_height + height_step * 2.0f }, width, height);
		ui_comp_->ui_list.insert({ "Weapon_Meele UI", meele_ui_ });

		// Grenade
		grenade_ui_ = make_shared<UI_Image>();
		grenade_ui_->InitImage("T_Weapon_Grenade.png");
		grenade_ui_->SetLocalRectByMin({ 100.0f, weapon_ui_height + height_step * 3.0f }, width, height);
		grenade_ammo_text_ = make_shared<UI_Text>();
		grenade_ui_->AddChildUI("1_AmmoText", grenade_ammo_text_);
		grenade_ammo_text_->InitText("5", ROBOTO, text_pos, text_size);
		ui_comp_->ui_list.insert({ "Weapon_Grenade UI", grenade_ui_ });

		// Weapon Select
		weapon_select_ui_ = make_shared<UI_Image>();
		weapon_select_ui_->InitImage("T_Weapon_Select.png");
		weapon_select_ui_->SetLocalRectByMin({ 100.0f, weapon_ui_height + height_step * 0.0f }, width, height);
		ui_comp_->ui_list.insert({ "Weapon_Select UI", weapon_select_ui_ });

		// Ammo
		ammo_ui_ = make_shared<UI_Image>();
		ammo_ui_->InitImage("T_Weapon_Ammo.png");
		ammo_ui_->SetLocalRectByMin({ 150.0f, weapon_ui_height + height_step * 4.0f + 50.0f }, 50.0f, 50.0f);

		ammo_cur_text_ = make_shared<UI_Text>();
		ammo_cur_text_->InitText("30", ROBOTO, { 70.0f, 5.0f }, 1.3f);
		ammo_ui_->AddChildUI("1_AmmoCurrent", ammo_cur_text_);

		ammo_max_text_ = make_shared<UI_Text>();
		ammo_max_text_->InitText("150", ROBOTO, { 130.0f, 15.0f }, 0.8f, {0.3f, 0.3f, 0.3f, 1.0f});
		ammo_ui_->AddChildUI("1_AmmoMax", ammo_max_text_);


		ui_comp_->ui_list.insert({ "Weapon_Ammo UI", ammo_ui_ });
	}

	// 상태 + 아이템 UI
	{
		status_ui = make_shared<UI_Image>();
		status_ui->InitImage("T_ItemUI.png");
		status_ui->SetLocalRectByCenter({ win_size_1920_width / 2.0f, win_size_1920_height - 150.0f }, 512.0f, 200.0f);

		inven_select_ = make_shared<UI_Image>();
		inven_select_->InitImage("T_Item_Select.png");
		inven_select_->SetLocalRectByCenter({ 101.5f, 76.5f }, 90.0f, 90.0f);
		status_ui->AddChildUI("2_InvenSelect", inven_select_);

		inven_[0] = make_shared<UI_Image>();
		inven_[0]->InitImage("");
		inven_[0]->SetLocalRectByCenter({ 101.5f, 76.5f }, 90.0f, 90.0f);
		inven_count_text_[0] = make_shared<UI_Text>();
		inven_count_text_[0]->InitText("", E_Font::ROTUNDA, { 70.0f, 70.0f }, 0.8f);
		inven_[0]->AddChildUI("2_InvenCountText", inven_count_text_[0]);
		inven_slot_text_[0] = make_shared<UI_Text>();
		inven_slot_text_[0]->InitText("1", E_Font::ROTUNDA, { 5.0f, 12.0f });
		inven_[0]->AddChildUI("2_InvenSlotText", inven_slot_text_[0]);
		inven_cooltime_img_[0] = make_shared<UI_Image>();
		inven_cooltime_img_[0]->InitImage("T_ItemCoolTime.png");
		inven_[0]->AddChildUI("3_CoolDownImg", inven_cooltime_img_[0]);
		inven_cooltime_img_[0]->SetLocalRectByMin({ 0.0f, 0.0f }, 90.0f, 90.0f);

		inven_[1] = make_shared<UI_Image>();
		inven_[1]->InitImage("");
		inven_[1]->SetLocalRectByCenter({ 203.5f, 76.5f }, 90.0f, 90.0f);
		inven_count_text_[1] = make_shared<UI_Text>();
		inven_count_text_[1]->InitText("", E_Font::ROTUNDA, { 70.0f, 70.0f }, 0.8f);
		inven_[1]->AddChildUI("2_InvenCountText", inven_count_text_[1]);
		inven_slot_text_[1] = make_shared<UI_Text>();
		inven_slot_text_[1]->InitText("2", E_Font::ROTUNDA, { 5.0f, 12.0f });
		inven_[1]->AddChildUI("2_InvenSlotText", inven_slot_text_[1]);
		inven_cooltime_img_[1] = make_shared<UI_Image>();
		inven_cooltime_img_[1]->InitImage("T_ItemCoolTime.png");
		inven_[1]->AddChildUI("3_CoolDownImg", inven_cooltime_img_[1]);
		inven_cooltime_img_[1]->SetLocalRectByMin({ 0.0f, 0.0f }, 90.0f, 90.0f);

		inven_[2] = make_shared<UI_Image>();
		inven_[2]->InitImage("");
		inven_[2]->SetLocalRectByCenter({ 306.5f, 76.5f }, 90.0f, 90.0f);
		inven_count_text_[2] = make_shared<UI_Text>();
		inven_count_text_[2]->InitText("", E_Font::ROTUNDA, { 70.0f, 70.0f }, 0.8f);
		inven_[2]->AddChildUI("2_InvenCountText", inven_count_text_[2]);
		inven_slot_text_[2] = make_shared<UI_Text>();
		inven_slot_text_[2]->InitText("3", E_Font::ROTUNDA, { 5.0f, 12.0f });
		inven_[2]->AddChildUI("2_InvenSlotText", inven_slot_text_[2]);
		inven_cooltime_img_[2] = make_shared<UI_Image>();
		inven_cooltime_img_[2]->InitImage("T_ItemCoolTime.png");
		inven_[2]->AddChildUI("3_CoolDownImg", inven_cooltime_img_[2]);
		inven_cooltime_img_[2]->SetLocalRectByMin({ 0.0f, 0.0f }, 90.0f, 90.0f);

		inven_[3] = make_shared<UI_Image>();
		inven_[3]->InitImage("");
		inven_[3]->SetLocalRectByCenter({ 410.5f, 76.5f }, 90.0f, 90.0f);
		inven_count_text_[3] = make_shared<UI_Text>();
		inven_count_text_[3]->InitText("", E_Font::ROTUNDA, { 70.0f, 70.0f }, 0.8f);
		inven_[3]->AddChildUI("2_InvenCountText", inven_count_text_[3]);
		inven_slot_text_[3] = make_shared<UI_Text>();
		inven_slot_text_[3]->InitText("4", E_Font::ROTUNDA, { 5.0f, 12.0f });
		inven_[3]->AddChildUI("2_InvenSlotText", inven_slot_text_[3]);
		inven_cooltime_img_[3] = make_shared<UI_Image>();
		inven_cooltime_img_[3]->InitImage("T_ItemCoolTime.png");
		inven_[3]->AddChildUI("3_CoolDownImg", inven_cooltime_img_[3]);
		inven_cooltime_img_[3]->SetLocalRectByMin({ 0.0f, 0.0f }, 90.0f, 90.0f);

		hp_img_ = make_shared<UI_Image>();
		status_ui->AddChildUI("1_HpBar", hp_img_);
		hp_img_->InitImage("T_HpBar.png");
		hp_img_->SetLocalRectByMin({ status_ui->rect_transform_[E_Resolution::R1920x1080].world_rect.width / 2.0f - 200.0f, 150.0f }, 400.0f, 30.0f);
		ui_comp_->ui_list.insert({ "Status UI", status_ui });
	}

	// 미니맵 UI
	{
		minimap_ui = make_shared<UI_Minimap>();
		minimap_ui->InitMinimap("T_Minimap.png");
		minimap_ui->SetLocalRectByMax({ win_size_1920_width, 300.0f }, 300.0f, 300.0f);

		auto border_image = make_shared<UI_Image>();
		minimap_ui->AddChildUI("1_MinimapBorder", border_image);
		border_image->InitImage("T_Minimap_Border.png");
		border_image->SetLocalRectByMin({ 0,0 }, 300.0f, 300.0f);
		ui_comp_->ui_list.insert({ "Minimap UI", minimap_ui });
	}

	// Objective UI
	{
		objective_ui_ = make_shared<UI_Image>();
		objective_ui_->InitImage("T_ObjectiveUI.png");
		objective_ui_->SetLocalRectByMax({ win_size_1920_width, 600.0f }, 300.0f, 300.0f);

		wave_text_ = make_shared<UI_Text>();
		wave_text_->InitText("Wave 1", E_Font::ROBOTO, { 123.0f, 50.0f }, 0.6f);
		objective_ui_->AddChildUI("1_WaveText", wave_text_);

		time_image_ = make_shared<UI_Image>();
		time_image_->InitImage("T_Time_Img.png");
		objective_ui_->AddChildUI("1_DayIcon", time_image_);
		time_image_->SetLocalRectByCenter({ 144.0f, 84.9f }, 17.0f, 17.0f);

		time_text_ = make_shared<UI_Text>();
		time_text_->InitText("10", E_Font::ROBOTO, { 130.5f, 100.0f }, 0.4f);
		objective_ui_->AddChildUI("1_TimeText", time_text_);

		duration_bar_ = make_shared<UI_Image>();
		duration_bar_->InitImage("T_HpBar.png");
		objective_ui_->AddChildUI("1_DurationBar", duration_bar_);
		duration_bar_->SetLocalRectByMin({ 31.8f, 141.0f }, 239.1f, 16.0f);

		duration_text_ = make_shared<UI_Text>();
		duration_text_->InitText("0 / 12", E_Font::ROBOTO, { 140.0f, 160.0f }, 0.5f);
		objective_ui_->AddChildUI("1_DurationText", duration_text_);

		repair_bar_ = make_shared<UI_Image>();
		repair_bar_->InitImage("T_HpBar.png");
		objective_ui_->AddChildUI("1_RepairBar", repair_bar_);
		repair_bar_->SetLocalRectByMin({ 31.8f, 205.2f }, 239.1f, 16.0f);

		repair_text_ = make_shared<UI_Text>();
		repair_text_->InitText("0 / 12", E_Font::ROBOTO, { 130.5f, 225.0f }, 0.5f);
		objective_ui_->AddChildUI("1_RepairText", repair_text_);

		kill_text_ = make_shared<UI_Text>();
		kill_text_->InitText("300", E_Font::ROBOTO, { 155.0f, 258.0f }, 0.7f);
		objective_ui_->AddChildUI("1_KillText", kill_text_);

		ui_comp_->ui_list.insert({ "Objective UI", objective_ui_ });
	}

	// CrossHair UI
	crosshair_ui_ = make_shared<UI_Image>();
	crosshair_ui_->InitImage("T_DotCrossHair.png");
	crosshair_ui_->SetLocalRectByCenter({ win_size_1920_width / 2.0f, win_size_1920_height / 2.0f }, 8.0f, 8.0f);
	ui_comp_->ui_list.insert({ "CrossHair UI", crosshair_ui_ });

	// Interaction UI
	{
		interaction_ui_ = make_shared<UI_Image>();
		interaction_ui_->InitImage("T_Interaction.png");
		interaction_ui_->SetLocalRectByMin({ win_size_1920_width * 3.0f / 5.0f, win_size_1920_height / 2.0f }, 200.0f, 40.0f);

		interaction_progressbar_ = make_shared<UI_Image>();
		interaction_progressbar_->InitImage("T_Interaction_Progress.png");
		interaction_progressbar_->SetLocalRectByMin({ 0.0f, 0.0f }, 200.0f, 40.0f);

		interaction_icon_ = make_shared<UI_Image>();
		interaction_icon_->InitImage("T_E_Icon.png");
		interaction_icon_->SetLocalRectByMin({ 2.5f, 2.5f }, 35.0f, 35.0f);
		interaction_ui_->AddChildUI("2_InteractionIcon", interaction_icon_);

		interaction_text_ = make_shared<UI_Text>();
		interaction_text_->InitText("", E_Font::ROBOTO, { 40.0f, 10.0f }, 0.5f);
		interaction_ui_->AddChildUI("2_InteractionText", interaction_text_);
	}

	// Addicted UI
	{
		addicted_ui_ = make_shared<UI_Image>();
		addicted_ui_->InitImage("T_Addicted.png");
		addicted_ui_->SetLocalRectByMin({ 0.0f, 0.0f }, win_size_1920_width, win_size_1920_height);
		//ui_comp_->ui_list.insert({ "Addicted UI", addicted_ui_ });
	}

	// Event Message UI
	event_msg_text_ = make_shared<UI_Text>();
	event_msg_text_->InitText("Event Test", E_Font::ROBOTO, { win_size_1920_width / 2.0f - 100.0f, win_size_1920_height - 300.0f });
	
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
	menu_window_->AddChildUI("1_MenuTitle", menu_title_);
	menu_title_->SetLocalRectByCenter({ win_size_1920_width / 2.0f, win_size_1920_height / 3.0f }, 897.0f, 141.0f);

	float height_step = 100.0f;

	// Menu Resume Button
	menu_resume_button_ = make_shared<UI_Button>();
	menu_resume_button_->InitButton("T_Ingame_But_Resume_Normal.png", "T_Ingame_But_Resume_Hover.png");
	menu_window_->AddChildUI("1_ResumeButton", menu_resume_button_);
	menu_resume_button_->SetLocalRectByCenter({ win_size_1920_width / 2.0f, win_size_1920_height / 2.0f + height_step * 0.0f }, 269.0f, 54.0f);

	// Menu Option Button
	menu_option_button_ = make_shared<UI_Button>();
	menu_option_button_->InitButton("T_Ingame_But_Option_Normal.png", "T_Ingame_But_Option_Hover.png");
	menu_window_->AddChildUI("1_OptionButton", menu_option_button_);
	menu_option_button_->SetLocalRectByCenter({ win_size_1920_width / 2.0f, win_size_1920_height / 2.0f + height_step * 1.0f}, 269.0f, 54.0f);

	// Menu Option Window
	menu_option_window_ = make_shared<UI_OptionWindow>();
	menu_option_window_->InitOptionWindow();

	// Menu Exit Button
	menu_exit_button_ = make_shared<UI_Button>();
	menu_exit_button_->InitButton("T_Ingame_But_Exit_Normal.png", "T_Ingame_But_Exit_Hover.png");
	menu_window_->AddChildUI("1_ExitButton", menu_exit_button_);
	menu_exit_button_->SetLocalRectByCenter({ win_size_1920_width / 2.0f, win_size_1920_height / 2.0f + height_step * 2.0f }, 269.0f, 54.0f);
}

void UI_Actor_Ingame::UpdateIngameUI()
{
	auto ingame_scene = (InGameScene*)SCENE_MGR->GetScene(INGAME).get();

	float win_size_1920_width = E_Resolution_Size[E_Resolution::R1920x1080].x;
	float win_size_1920_height = E_Resolution_Size[E_Resolution::R1920x1080].y;

	// Time Icon Move Update
	{
		static float bar_init_width = duration_bar_->rect_transform_[R1920x1080].local_rect.width;
		auto& enviroment = ingame_scene->GetEnviroment();
		float y = time_image_->rect_transform_[R1920x1080].local_rect.center.y;
		float width = time_image_->rect_transform_[R1920x1080].local_rect.width;
		float height = time_image_->rect_transform_[R1920x1080].local_rect.height;

		float left = 54.6f;
		float right = 247.5f;
		float center = 151.5f;
		float time = enviroment.GetCountingTime();
		XMFLOAT2 time_limit = enviroment.GetTimeLimits();
		// Night
		if (time < 0.0f)
		{
			float length = right - center;
			float x = center + length * time / time_limit.y;
			time_image_->SetLocalRectByCenter({ x, y, }, width, height);
			;
		}
		// Day
		else
		{
			float length = center - left;
			float x = center - length * time / time_limit.x;
			time_image_->SetLocalRectByCenter({ x, y, }, width, height);
		}

	}

	// HP Update
	{
		player_ = SCENE_MGR->GetPlayer<Player>(0);
		float hp_ratio = (float)player_->GetCurHp() / (float)player_->GetMaxHp();
		if (hp_ratio > 0.3f)
			hp_img_->render_data_.tex_id = "T_HpBar.png";
		else
			hp_img_->render_data_.tex_id = "T_HpBarRed.png";

		hp_img_->SetLocalRectByMin({ status_ui->rect_transform_[E_Resolution::R1920x1080].world_rect.width / 2.0f - 200.0f, 150.0f }, 400.0f * hp_ratio, 30.0f);
	}

	// Wave / Time Update
	{
		string wave_text = "Wave ";
		wave_text += to_string(ingame_scene->GetWaveSystem().wave_count_);
		wave_text_->SetText(wave_text);

		string time_str = to_string(wave_timer_);
		time_str = time_str.substr(0, time_str.find('.') + 2 + 1);
		time_text_->SetText(time_str);
	}

	// Kill Update
	kill_text_->SetText(to_string(player_->GetKillScore()));

	// Weapon UI Update
	{
		float weapon_ui_height = win_size_1920_height - 500.0f;
		float height_step = 78.0f;
		float width = weapon_select_ui_->rect_transform_[R1920x1080].world_rect.width;
		float height = weapon_select_ui_->rect_transform_[R1920x1080].world_rect.height;
		
		int cur_weapon = (int)player_->cur_equipped_weapon_;

		weapon_select_ui_->SetLocalRectByMin({ 100.0f, weapon_ui_height + height_step * cur_weapon }, width, height);

		int total_ar_ammo = player_->cur_weapon_using_remained_[(int)EQUIPPED_WEAPON::AUTO_RIFLE] +
			player_->cur_weapon_total_remained_[(int)EQUIPPED_WEAPON::AUTO_RIFLE];
		int total_hand_gun_ammo = player_->cur_weapon_using_remained_[(int)EQUIPPED_WEAPON::HAND_GUN] +
			player_->cur_weapon_total_remained_[(int)EQUIPPED_WEAPON::HAND_GUN];
		int total_grenade_ammo = player_->cur_weapon_using_remained_[(int)EQUIPPED_WEAPON::GRENADE] +
			player_->cur_weapon_total_remained_[(int)EQUIPPED_WEAPON::GRENADE];

		ar_ammo_text_->SetText(to_string(total_ar_ammo));
		handgun_ammo_text_->SetText(to_string(total_hand_gun_ammo));
		grenade_ammo_text_->SetText(to_string(total_grenade_ammo));

		ammo_cur_text_->SetText(to_string( player_->cur_weapon_using_remained_[cur_weapon]));
		ammo_max_text_->SetText(to_string( player_->cur_weapon_total_remained_[cur_weapon]));
	}


	// Vehicle Duration Update
	{
		static float bar_init_width = duration_bar_->rect_transform_[R1920x1080].local_rect.width;
		auto ingame_scene = (InGameScene*)SCENE_MGR->GetScene(INGAME).get();
		auto& wave_sys = ingame_scene->GetWaveSystem();

		// if health is under 30, Red
		if (wave_sys.car_health < 30.0f)
			duration_bar_->SetImage("T_HpBarRed.png");
		else
			duration_bar_->SetImage("T_HpBar.png");

		// Set bar width with health
		XMFLOAT2 min = duration_bar_->rect_transform_[R1920x1080].local_rect.min;
		float width = bar_init_width * wave_sys.car_health / 100.0f;
		float height = duration_bar_->rect_transform_[R1920x1080].local_rect.height;
		duration_bar_->SetLocalRectByMin(min, width, height);

		string car_health_str = to_string(wave_sys.car_health);
		duration_text_->SetText(car_health_str);
	}

	// Vehicle Parts Update
	{
		static float bar_init_width = repair_bar_->rect_transform_[R1920x1080].local_rect.width;
		auto ingame_scene = (InGameScene*)SCENE_MGR->GetScene(INGAME).get();
		auto& wave_sys = ingame_scene->GetWaveSystem();

		// Set bar width with health
		XMFLOAT2 min = repair_bar_->rect_transform_[R1920x1080].local_rect.min;
		float width = bar_init_width * wave_sys.car_repair_count / 12;
		float height = repair_bar_->rect_transform_[R1920x1080].local_rect.height;
		repair_bar_->SetLocalRectByMin(min, width, height);

		string repair_str = to_string(wave_sys.car_repair_count) + " / 12";
		repair_text_->SetText(repair_str);
	}

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
		// Item Selection
		Rect rect;
		switch (player_->selected_slot)
		{
		case 0:
			rect = inven_[0]->rect_transform_[E_Resolution::R1920x1080].local_rect;
			break;
		case 1:
			rect = inven_[1]->rect_transform_[E_Resolution::R1920x1080].local_rect;
			break;
		case 2:
			rect = inven_[2]->rect_transform_[E_Resolution::R1920x1080].local_rect;
			break;
		case 3:
			rect = inven_[3]->rect_transform_[E_Resolution::R1920x1080].local_rect;
			break;
		}

		inven_select_->SetLocalRectByCenter(rect.center, rect.width, rect.height);

		// Item Update
		auto& inventory = player_->GetInventory();
		auto& inventory_timer = player_->GetInventoryTimer();
		for (int i = 0; i < INVENTORY_MAX; i++)
		{
			if (inventory[i] == NULL)
			{
				status_ui->DeleteChildUI("1_inven" + to_string(i));
			}
			else
			{
				status_ui->AddChildUI("1_inven"+ to_string(i), inven_[i]);
				inven_[i]->SetImage(inventory[i]->GetIcon());
				inven_count_text_[i]->SetText(to_string(inventory[i]->GetCount()));

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
				interaction_ui_->DeleteChildUI("1_InteractionProgressBar");

			}
			// Extract Car Interaction
			if (player_->can_extract_repair_)
			{
				interaction_text_->SetText("Push to Extract");
				interaction_ui_->AddChildUI("1_InteractionProgressBar", interaction_progressbar_);
				float width = 200.0f * player_->extract_during_time_ / player_->extract_time_takes_;
				interaction_progressbar_->SetLocalRectByMin({ 0.0f, 0.0f }, width, 40.0f);
			}
			// Repair Car Interaction
			if (player_->can_repair_car_)
			{
				interaction_text_->SetText("Push to Repair");
				interaction_ui_->AddChildUI("1_InteractionProgressBar", interaction_progressbar_);
				float width = 200.0f * player_->repair_during_time_ / player_->repair_time_takes_;
				interaction_progressbar_->SetLocalRectByMin({ 0.0f, 0.0f }, width, 40.0f);
			}
		}
		else if((player_->selectable_counts_ == 0 && !player_->can_extract_repair_) && ui_comp_->ui_list.find("Interaction UI") != ui_comp_->ui_list.end())
			ui_comp_->ui_list.erase("Interaction UI");
	}

	// EventMessageText Update
	if (event_msg_timer_ < event_msg_time_)
	{
		event_msg_timer_ += TIMER->GetDeltaTime();
	}
	else if(ui_comp_->ui_list.find("Event Message UI") != ui_comp_->ui_list.end())
	{
		event_msg_text_->SetText("");
		ui_comp_->ui_list.erase("Event Message UI");
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
		ENGINE->Resize((E_Resolution)(menu_option_window_->option_resolution_list_box_->GetCurrentIndex() + 1));
		CloseOptionWindow();
	}
}

void UI_Actor_Ingame::SetEventMsg(string msg)
{
	event_msg_text_->SetText(msg);
	event_msg_timer_ = 0.0f; 
	ui_comp_->ui_list.insert({ "Event Message UI", event_msg_text_ });
}

void UI_Actor_Ingame::OpenMenu()
{
	ui_comp_->ui_list.insert({ "Menu UI", menu_window_ });
	
	ui_comp_->ui_list.erase("Weapon_AR UI");
	ui_comp_->ui_list.erase("Weapon_Handgun UI");
	ui_comp_->ui_list.erase("Weapon_Meele UI");
	ui_comp_->ui_list.erase("Weapon_Grenade UI");
	ui_comp_->ui_list.erase("Weapon_Select UI");
	ui_comp_->ui_list.erase("Weapon_Ammo UI");
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

	ui_comp_->ui_list.insert({ "Weapon_AR UI", ar_ui_ });
	ui_comp_->ui_list.insert({ "Weapon_Handgun UI", handgun_ui_ });
	ui_comp_->ui_list.insert({ "Weapon_Meele UI", meele_ui_ });
	ui_comp_->ui_list.insert({ "Weapon_Grenade UI", grenade_ui_});
	ui_comp_->ui_list.insert({ "Weapon_Select UI", weapon_select_ui_ });
	ui_comp_->ui_list.insert({ "Weapon_Ammo UI", ammo_ui_ });

	ui_comp_->ui_list.insert({ "Minimap UI", minimap_ui });
	ui_comp_->ui_list.insert({ "Status UI", status_ui });
	ui_comp_->ui_list.insert({ "Objective UI", objective_ui_ });
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

