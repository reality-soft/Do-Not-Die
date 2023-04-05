#include "stdafx.h"
#include "UI_Actor_Ingame.h"
#include "SceneMgr.h"

using namespace reality;

void UI_Actor_Ingame::OnInit(entt::registry& registry)
{
	UIActor::OnInit(registry);

	CreateUI();

}

void UI_Actor_Ingame::OnUpdate()
{
	UpdateUI();
}

void UI_Actor_Ingame::CreateUI()
{
	C_UI& ui_comp = reg_scene_->get<C_UI>(GetEntityId());

	// 무기 UI
	weapon_ui_ = make_shared<UI_Image>();
	weapon_ui_->InitImage("T_Handgun_01.png");
	weapon_ui_->SetLocalRectByMin({ 100.0f, ENGINE->GetWindowSize().y - 200.0f }, 512.0f, 179.0f);
	ammo_text_ = make_shared<UI_Text>();
	weapon_ui_->AddChildUI(ammo_text_);
	ammo_text_->InitText("30", BASIC, { 320.0f, 72.0f }, 1.5f);
	ui_comp.ui_list.insert({ "Weapon UI", weapon_ui_ });

	// 상태 + 아이템 UI
	status_ui = make_shared<UI_Image>();
	status_ui->InitImage("T_ItemUI.png");
	status_ui->SetLocalRectByCenter({ ENGINE->GetWindowSize().x / 2.0f, ENGINE->GetWindowSize().y - 150.0f }, 512.0f, 200.0f);

	hp_img_ = make_shared<UI_Image>();
	status_ui->AddChildUI(hp_img_);
	hp_img_->InitImage("T_HpBar.png");
	hp_img_->SetLocalRectByMin({ status_ui->rect_transform_.world_rect.width / 2.0f - 200.0f, 150.0f }, 400.0f, 30.0f);
	ui_comp.ui_list.insert({ "Status UI", status_ui });

	// 미니맵 UI
	minimap_ui = make_shared<UI_Minimap>();
	minimap_ui->InitMinimap("T_Minimap.png");
	minimap_ui->SetLocalRectByMin({ ENGINE->GetWindowSize().x * 4.0f / 5.0f, ENGINE->GetWindowSize().y * 1.0f / 10.0f }, 300.0f, 300.0f);

	auto border_image = make_shared<UI_Image>();
	minimap_ui->AddChildUI(border_image);
	border_image->InitImage("T_Minimap_Border.png");
	border_image->SetLocalRectByMin({ 0,0 }, 300.0f, 300.0f);
	ui_comp.ui_list.insert({ "Minimap UI", minimap_ui });

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

	ui_comp.ui_list.insert({ "time UI", time_ui_ });


	// Objective UI
	objective_ui_ = make_shared<UI_Image>();
	objective_ui_->InitImage("T_ObjectiveUI.png");
	objective_ui_->SetLocalRectByMin({ ENGINE->GetWindowSize().x * 4.0f / 5.0f, ENGINE->GetWindowSize().y * 4.0f / 10.0f }, 323.0f, 387.0f);

	tire_text_ = make_shared<UI_Text>();
	tire_text_->InitText("TIRE : 10 / 30", E_Font::ROTUNDA, { 50.0f, 140.0f });
	objective_ui_->AddChildUI(tire_text_);

	steel_text_ = make_shared<UI_Text>();
	steel_text_->InitText("STEEL : 10 / 30", E_Font::ROTUNDA, { 50.0f, 200.0f });
	objective_ui_->AddChildUI(steel_text_);

	ui_comp.ui_list.insert({ "Objective UI", objective_ui_ });

	// CrossHair UI
	crosshair_ui_ = make_shared<UI_Image>();
	crosshair_ui_->InitImage("T_DotCrossHair.png");
	crosshair_ui_->SetLocalRectByCenter({ ENGINE->GetWindowSize().x / 2.0f, ENGINE->GetWindowSize().y / 2.0f }, 8.0f, 8.0f);
	ui_comp.ui_list.insert({ "CrossHair UI", crosshair_ui_ });
}

void UI_Actor_Ingame::UpdateUI()
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

