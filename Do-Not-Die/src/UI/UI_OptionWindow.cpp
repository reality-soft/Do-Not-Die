#include "stdafx.h"
#include "UI_OptionWindow.h"
#include "Engine.h"
#include "FmodMgr.h"

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

	// Resolution Listbox
	option_resolution_list_box_ = make_shared<UI_Listbox>();
	option_resolution_list_box_->InitListBox("T_Listbox_OpenPanel.png", "T_Listbox_Closed.png", "T_Listbox_Opened.png");
	option_resolution_list_box_->InitItemButton({ 35.0f, 35.0f }, 200.0f, 30.0f, 40.0f, "T_Listbox_Item_Normal.png", "T_Listbox_Item_Hover.png", "T_Listbox_Item_Normal.png");
	option_window_->AddChildUI(option_resolution_list_box_);
	option_resolution_list_box_->SetLocalRectByMin({ 40.0f, 180.0f }, 265.0f, 51.0f);
	option_resolution_list_box_->SetOpenPanelLocalRectByMin({ 0.0f, option_resolution_list_box_->rect_transform_.world_rect.height - 20.0f }, 263.0f, 181.0f);
	option_resolution_list_box_->AddItem("1920 x 1080");
	option_resolution_list_box_->AddItem("800 x 600");
	option_resolution_list_box_->SetItemSelected(0);


	// Volume Slider
	option_volume_slider_ = make_shared<UI_Slider>();
	option_volume_slider_->InitSlider("T_Volume_Slider_back.png", "T_Volume_Slider.png", "T_Volume_Slider.png");
	option_window_->AddChildUI(option_volume_slider_);
	option_volume_slider_->SetLocalRectByMin({50.0f, 430.0f}, 265.0f, 51.0f);
	option_volume_slider_->SetSliderLocalRect(44.0f, 18.0f);
}

void UI_OptionWindow::UpdateThisUI()
{
	FMOD_MGR->SetMusicVolume(option_volume_slider_->GetValue() / 10.0f);
	FMOD_MGR->SetSFXVolume(option_volume_slider_->GetValue() / 10.0f);
}

E_UIState UI_OptionWindow::GetCloseButtonState()
{
	return option_close_button_->GetCurrentState();
}

void UI_OptionWindow::SetCloseButtonState(E_UIState state)
{
	option_close_button_->SetCurrentState(state);
}
