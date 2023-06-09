#include "stdafx.h"
#include "UI_Actor_Title.h"
#include "SceneMgr.h"
#include "InGameScene.h"
#include "StartScene.h"

using namespace reality;

void UI_Actor_Title::OnInit(entt::registry& registry)
{
	UIActor::OnInit(registry);

	CreateUI();

}

void UI_Actor_Title::OnUpdate()
{
	UpdateUI();
}

void UI_Actor_Title::CreateUI()
{
	ui_comp_ = &reg_scene_->get<C_UI>(GetEntityId());

	float win_size_1920_width = E_Resolution_Size[E_Resolution::R1920x1080].x;
	float win_size_1920_height = E_Resolution_Size[E_Resolution::R1920x1080].y;
	
	// Title Image
	title_img_ = make_shared<UI_Image>();
	title_img_->InitImage("T_GameTitle.png");
	title_img_->SetLocalRectByMin({ win_size_1920_width * 2.0f / 4.0f - 100.0f, win_size_1920_height * 1.0f / 5.0f }, 1000.0f, 300.0f);
	ui_comp_->ui_list.insert({ "1_Title Image", title_img_ });

	// Background UI
	background_img_ = make_shared<UI_Image>();
	background_img_->InitImage("T_Intro_Background.png");
	background_img_->SetLocalRectByMin({ 0, 0 }, win_size_1920_width, win_size_1920_height);
	ui_comp_->ui_list.insert({ "Background Image", background_img_ });

	float button_x = title_img_->rect_transform_[R1920x1080].world_rect.center.x;

	float img_width = 200.0f;
	float img_height = 200.0f;

	// Direct X IMG
	direct_x_img_ = make_shared<UI_Image>();
	direct_x_img_->InitImage("T_Directx11.png");
	direct_x_img_->SetLocalRectByMin({ 50.0f, win_size_1920_height * 5.0f / 6.0f }, img_width, img_height);
	ui_comp_->ui_list.insert({ "DirectX Image", direct_x_img_ });

	// FMOD IMG
	fmod_img_ = make_shared<UI_Image>();
	fmod_img_->InitImage("T_Fmod.png");
	fmod_img_->SetLocalRectByMin({ 50.0f + img_width + 30.0f, win_size_1920_height * 5.0f / 6.0f }, img_width, img_height);
	ui_comp_->ui_list.insert({ "FMOD Image", fmod_img_ });

	// KGCA IMG
	kgca_img_ = make_shared<UI_Image>();
	kgca_img_->InitImage("T_KGCA.png");
	kgca_img_->SetLocalRectByMin({ 50.0f + img_width * 2.0f + 70.0f, win_size_1920_height * 5.0f / 6.0f }, img_width, img_height);
	ui_comp_->ui_list.insert({ "KGCA Image", kgca_img_ });

	// New Game Button
	newgame_button_ = make_shared<UI_Button>();
	newgame_button_->InitButton("T_Button_NewGame_Normal.png", "T_Button_NewGame_Hover.png", "T_Button_NewGame_Pushed.png");
	newgame_button_->SetButtonSound("S_UI_Button_Hover.wav", "S_UI_Button_Push.wav", "S_UI_Button_Select.wav");
	newgame_button_->SetLocalRectByCenter({ button_x, win_size_1920_height * 3.0f / 5.0f }, 200.0f, 70.0f);
	ui_comp_->ui_list.insert({ "1_New Game Button", newgame_button_ });

	// Option Button
	option_button_ = make_shared<UI_Button>();
	option_button_->InitButton("T_Button_Option_Normal.png", "T_Button_Option_Hover.png", "T_Button_Option_Pushed.png");
	option_button_->SetButtonSound("S_UI_Button_Hover.wav", "S_UI_Button_Push.wav", "S_UI_Button_Select.wav");
	option_button_->SetLocalRectByCenter({ button_x, win_size_1920_height * 3.0f / 5.0f + 70.0f * 1.0f }, 200.0f, 70.0f);
	ui_comp_->ui_list.insert({ "1_Option Button", option_button_ });

	// Exit Button
	exit_button_ = make_shared<UI_Button>();
	exit_button_->InitButton("T_Button_Exit_Normal.png", "T_Button_Exit_Hover.png", "T_Button_Exit_Pushed.png");
	exit_button_->SetButtonSound("S_UI_Button_Hover.wav", "S_UI_Button_Push.wav", "S_UI_Button_Select.wav");
	exit_button_->SetLocalRectByCenter({ button_x, win_size_1920_height * 3.0f / 5.0f + 70.0f * 2.0f }, 200.0f, 70.0f);
	ui_comp_->ui_list.insert({ "1_Exit Button", exit_button_ });

	// Option Window
	option_window_ = make_shared<UI_OptionWindow>();
	option_window_->InitOptionWindow();

	title_img_->SetAlpha(0.0f);
	background_img_->SetAlpha(0.0f); 
	direct_x_img_->SetAlpha(0.0f);
	fmod_img_->SetAlpha(0.0f); 
	kgca_img_->SetAlpha(0.0f);
	newgame_button_->SetAlpha(0.0f);
	//loadinggame_button->SetAlpha(0.0f);
	option_button_->SetAlpha(0.0f);
	exit_button_->SetAlpha(0.0f);

	newgame_button_->Off();
	//loadinggame_button->Off();
	option_button_->Off();
	exit_button_->Off();
}

void reality::UI_Actor_Title::FadeInUI()
{
	static float timer = 0.0f;
	static float alpha = 0.0f;
	static bool executed = false;
	if (alpha >= 1.0f)
	{
		if (!executed)
		{
			newgame_button_->On();
			//loadinggame_button->On();
			option_button_->On();
			exit_button_->On();
			executed = true;
		}
		return;
	}

	timer += TM_DELTATIME;
	alpha = FadeInAlpha(15.0f, 18.0f, timer);
	
	title_img_->SetAlpha(alpha);
	direct_x_img_->SetAlpha(alpha);
	fmod_img_->SetAlpha(alpha);
	kgca_img_->SetAlpha(alpha);
	newgame_button_->SetAlpha(alpha);
	//loadinggame_button->SetAlpha(alpha);
	option_button_->SetAlpha(alpha);
	exit_button_->SetAlpha(alpha);
}

void reality::UI_Actor_Title::DisappearUI()
{
	static float timer = 0.0f;
	static float alpha = 1.0f;

	timer += TM_DELTATIME;
	alpha = FadeOutAlpha(0.0f, 3.0f, timer);

	background_img_->SetAlpha(1.0f - alpha);
	direct_x_img_->SetAlpha(alpha);
	fmod_img_->SetAlpha(alpha);
	title_img_->SetAlpha(alpha);
	kgca_img_->SetAlpha(alpha);
	newgame_button_->SetAlpha(alpha);
	//loadinggame_button->SetAlpha(alpha);
	option_button_->SetAlpha(alpha);
	exit_button_->SetAlpha(alpha);

	newgame_button_->Off();
	//loadinggame_button->Off();
	option_button_->Off();
	exit_button_->Off();
}


void UI_Actor_Title::UpdateUI()
{
	FadeInUI();

	// When NewGame Button Selected
	if (newgame_button_->GetCurrentState() == E_UIState::UI_SELECT)
	{
		StartScene* start_scene = (StartScene*)(SCENE_MGR->GetScene(START).get());
		if (start_scene != nullptr)
			start_scene->scene_finished = true;
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
	if (ui_comp_->ui_list.find("2_Option Window") != ui_comp_->ui_list.end())
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

void UI_Actor_Title::OpenOptionWindow()
{
	newgame_button_->Off();
	//loadinggame_button->Off();
	option_button_->Off();
	exit_button_->Off();

	option_button_->SetCurrentState(E_UIState::UI_NORMAL);

	ui_comp_->ui_list.insert({ "2_Option Window", option_window_ });
}

void UI_Actor_Title::CloseOptionWindow()
{
	newgame_button_->On();
	//loadinggame_button->On();
	option_button_->On();
	exit_button_->On();

	option_window_->SetCloseButtonState(E_UIState::UI_NORMAL);

	ui_comp_->ui_list.erase("2_Option Window");
}