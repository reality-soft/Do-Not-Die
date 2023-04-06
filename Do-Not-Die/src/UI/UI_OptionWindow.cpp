#include "stdafx.h"
#include "UI_OptionWindow.h"
#include "Engine.h"

using namespace reality;

void UI_OptionWindow::InitOptionWindow()
{
	float window_size_width = ENGINE->GetWindowSize().x;
	float window_size_height = ENGINE->GetWindowSize().y;

	InitImage("T_Opaque.png");
	SetLocalRectByMin({ 0.0f, 0.0f }, window_size_width, window_size_height);

	// Option Window
	option_window_ = make_shared<UI_Image>();
	option_window_->InitImage("T_Option.png");
	AddChildUI(option_window_);
	option_window_->SetLocalRectByCenter({ window_size_width / 2.0f, window_size_height / 2.0f }, 246.0f * 2.0f, 305.0f * 2.0f);

	// Option Close Button
	option_close_button_ = make_shared<UI_Button>();
	option_close_button_->InitButton("T_Button_Close_Normal.png", "T_Button_Close_Hover.png", "T_Button_Close_Normal.png");
	option_window_->AddChildUI(option_close_button_);
	option_close_button_->SetLocalRectByCenter({ 370.0f, 80.0f }, 51.0f, 56.0f);
}

E_UIState UI_OptionWindow::GetCloseButtonState()
{
	return option_close_button_->GetCurrentState();
}

void UI_OptionWindow::SetCloseButtonState(E_UIState state)
{
	option_close_button_->SetCurrentState(state);
}
