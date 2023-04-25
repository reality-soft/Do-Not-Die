#include "stdafx.h"
#include "UI_Actor_StartScene.h"
#include "SceneMgr.h"
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

	float win_size_1920_width = E_Resolution_Size[E_Resolution::R1920x1080].x;
	float win_size_1920_height = E_Resolution_Size[E_Resolution::R1920x1080].y;
	
	// Title Image
	title_img_ = make_shared<UI_Image>();
	title_img_->InitImage("T_GameTitle.png");
	title_img_->SetLocalRectByMin({ win_size_1920_width * 2.0f / 4.0f - 100.0f, win_size_1920_height * 1.0f / 5.0f }, 1000.0f, 300.0f);
	ui_comp_->ui_list.insert({ "Title Image", title_img_ });

	float button_x = title_img_->rect_transform_[R1920x1080].world_rect.center.x;

	// New Game Button
	newgame_button_ = make_shared<UI_Button>();
	newgame_button_->InitButton("T_Button_NewGame_Normal.png", "T_Button_NewGame_Hover.png", "T_Button_NewGame_Pushed.png");
	newgame_button_->SetLocalRectByCenter({ button_x, win_size_1920_height * 3.0f / 5.0f }, 200.0f, 70.0f);
	ui_comp_->ui_list.insert({ "New Game Button", newgame_button_ });

	// Load Game Button
	loadinggame_button = make_shared<UI_Button>();
	loadinggame_button->InitButton("T_Button_LoadGame_Normal.png", "T_Button_LoadGame_Hover.png", "T_Button_LoadGame_Pushed.png");
	loadinggame_button->SetLocalRectByCenter({ button_x, win_size_1920_height * 3.0f / 5.0f + 70.0f * 1.0f }, 200.0f, 70.0f);
	ui_comp_->ui_list.insert({ "Load Game Button", loadinggame_button });

	// Option Button
	option_button_ = make_shared<UI_Button>();
	option_button_->InitButton("T_Button_Option_Normal.png", "T_Button_Option_Hover.png", "T_Button_Option_Pushed.png");
	option_button_->SetLocalRectByCenter({ button_x, win_size_1920_height * 3.0f / 5.0f + 70.0f * 2.0f }, 200.0f, 70.0f);
	ui_comp_->ui_list.insert({ "Option Button", option_button_ });

	// Exit Button
	exit_button_ = make_shared<UI_Button>();
	exit_button_->InitButton("T_Button_Exit_Normal.png", "T_Button_Exit_Hover.png", "T_Button_Exit_Pushed.png");
	exit_button_->SetLocalRectByCenter({ button_x, win_size_1920_height * 3.0f / 5.0f + 70.0f * 3.0f }, 200.0f, 70.0f);
	ui_comp_->ui_list.insert({ "Exit Button", exit_button_ });

	// Option Window
	option_window_ = make_shared<UI_OptionWindow>();
	option_window_->InitOptionWindow();

	title_img_->SetAlpha(0.0f);
	newgame_button_->SetAlpha(0.0f);
	loadinggame_button->SetAlpha(0.0f);
	option_button_->SetAlpha(0.0f);
	exit_button_->SetAlpha(0.0f);

	newgame_button_->Off();
	loadinggame_button->Off();
	option_button_->Off();
	exit_button_->Off();
}

void UI_Actor_StartScene::UpdateUI()
{
	static float timer = 0.0f;
	static float alpha = 0.0f;
	static float speed = 0.3f;

	timer += TIMER->GetDeltaTime();

	if (8.0f < timer && alpha < 1.0f)
	{
		timer += TIMER->GetDeltaTime();
		alpha += speed * TIMER->GetDeltaTime();
		title_img_->SetAlpha(alpha);
		newgame_button_->SetAlpha(alpha);
		loadinggame_button->SetAlpha(alpha);
		option_button_->SetAlpha(alpha);
		exit_button_->SetAlpha(alpha);
	}
	else if (alpha > 1.0f)
	{
		title_img_->SetAlpha(1.0f);
		newgame_button_->SetAlpha(1.0f);
		loadinggame_button->SetAlpha(1.0f);
		option_button_->SetAlpha(1.0f);
		exit_button_->SetAlpha(1.0f);
		newgame_button_->On();
		loadinggame_button->On();
		option_button_->On();
		exit_button_->On();
	}

	// When NewGame Button Selected
	if (newgame_button_->GetCurrentState() == E_UIState::UI_SELECT)
	{
		SCENE_MGR->ChangeScene(E_SceneType::INGAME);
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
		if (option_window_->GetCloseButtonState() == E_UIState::UI_SELECT || DINPUT->GetKeyState(DIK_ESCAPE) == KEY_PUSH)
			CloseOptionWindow();
	}

	if (option_window_->resolution_value != option_window_->option_resolution_list_box_->GetCurrentItem())
	{
		option_window_->resolution_value = option_window_->option_resolution_list_box_->GetCurrentItem();
		ENGINE->Resize((E_Resolution)(option_window_->option_resolution_list_box_->GetCurrentIndex() + 1));
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

	ui_comp_->ui_list.insert({ "Option Window", option_window_ });
}

void UI_Actor_StartScene::CloseOptionWindow()
{
	newgame_button_->On();
	loadinggame_button->On();
	option_button_->On();
	exit_button_->On();

	option_window_->SetCloseButtonState(E_UIState::UI_NORMAL);

	ui_comp_->ui_list.erase("Option Window");
}