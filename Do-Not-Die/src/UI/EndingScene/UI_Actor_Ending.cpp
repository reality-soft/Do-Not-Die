#include "UI_Actor_Ending.h"

void UI_Actor_Ending::OnInit(entt::registry& registry)
{
	UIActor::OnInit(registry);

	C_UI& ui_comp = reg_scene_->get<C_UI>(GetEntityId());

	float win_size_1920_width = E_Resolution_Size[E_Resolution::R1920x1080].x;
	float win_size_1920_height = E_Resolution_Size[E_Resolution::R1920x1080].y;

	// Game Over UI
	game_clear_ui_ = make_shared<UI_Image>();
	game_clear_ui_->InitImage("T_GameOver_Background.png");
	game_clear_ui_->SetLocalRectByMin({ 0.0f, 0.0f }, win_size_1920_width, win_size_1920_height);

	game_clear_img_ = make_shared<UI_Image>();
	game_clear_img_->InitImage("T_GameClear_Image.png");
	game_clear_ui_->AddChildUI("GameClear Img", game_clear_img_);
	game_clear_img_->SetLocalRectByCenter({ win_size_1920_width / 2.0f, win_size_1920_height * 2.0f / 5.0f }, 1200.0f, 250.0f);

	game_clear_exit_button_ = make_shared<UI_Button>();
	game_clear_exit_button_->InitButton("T_Gameover_ExitBtn_Normal.png", "T_Gamss deover_ExitBtn_Hover.png");
	game_clear_ui_->AddChildUI("Exit Button", game_clear_exit_button_);
	game_clear_exit_button_->SetLocalRectByCenter({ win_size_1920_width / 2.0f, win_size_1920_height * 3.0f / 4.0f }, 500.0f, 150.0f);

	ui_comp.ui_list.insert({ "Game Clear UI", game_clear_ui_ });
}

void UI_Actor_Ending::OnUpdate()
{	
	ShowCursor(true);

	if (game_clear_exit_button_->GetCurrentState() == E_UIState::UI_SELECT)
	{
		DestroyWindow(ENGINE->GetWindowHandle());		
	}
}
