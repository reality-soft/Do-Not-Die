#include "stdafx.h"
#include "UI_OptionWindow.h"
#include "Engine.h"
#include "FmodMgr.h"

using namespace reality;

void UI_OptionWindow::InitOptionWindow()
{
	float win_size_1920_width = E_Resolution_Size[E_Resolution::R1920x1080].x;
	float win_size_1920_height = E_Resolution_Size[E_Resolution::R1920x1080].y;


	InitImage("T_Opaque.png");
	SetLocalRectByMin({ 0.0f, 0.0f }, win_size_1920_width, win_size_1920_height);

	// Option Window
	option_window_ = make_shared<UI_Image>();
	option_window_->InitImage("T_Option.png");
	AddChildUI("1_OptionWindow", option_window_);
	option_window_->SetLocalRectByCenter({ win_size_1920_width / 2.0f, win_size_1920_height / 2.0f }, 246.0f * 2.0f, 305.0f * 2.0f);

	// Option Close Button
	option_close_button_ = make_shared<UI_Button>();
	option_close_button_->InitButton("T_Button_Close_Normal.png", "T_Button_Close_Hover.png", "T_Button_Close_Normal.png");
	option_close_button_->SetButtonSound("S_UI_Button_Hover.wav", "S_UI_Button_Push.wav", "S_UI_Button_Select.wav");
	option_window_->AddChildUI("1_CloseButton", option_close_button_);
	option_close_button_->SetLocalRectByCenter({ 450.0f, 42.0f }, 60.0f, 60.0f);

	// Resolution Listbox
	option_resolution_list_box_ = make_shared<UI_Listbox>();
	option_resolution_list_box_->InitListBox("T_Listbox_OpenPanel.png", "T_Listbox_Closed.png", "T_Listbox_Opened.png");
	option_resolution_list_box_->InitItemButton({ 35.0f, 35.0f }, 200.0f, 30.0f, 40.0f, "T_Listbox_Item_Normal.png", "T_Listbox_Item_Hover.png", "T_Listbox_Item_Normal.png");
	option_window_->AddChildUI("1_ResolutionListbox", option_resolution_list_box_);
	option_resolution_list_box_->SetLocalRectByMin({ 40.0f, 180.0f }, 265.0f, 51.0f);

	option_resolution_list_box_->SetOpenPanelLocalRectByMin({ 0.0f, option_resolution_list_box_->rect_transform_[E_Resolution::R1920x1080].world_rect.height - 20.0f}, 265.0f, 181.0f);

	option_resolution_list_box_->AddItem(E_Resolution_String[E_Resolution::R1920x1080]);
	option_resolution_list_box_->AddItem(E_Resolution_String[E_Resolution::R1280x720]);
	option_resolution_list_box_->SetItemSelected(E_Resolution_String[E_Resolution::R1920x1080]);
	option_resolution_list_box_->SetButtonSound("S_UI_Button_Hover.wav", "S_UI_Button_Push.wav", "S_UI_Button_Select.wav");
	option_resolution_list_box_->SetItemButtonSound("S_UI_Button_Hover.wav", "S_UI_Button_Push.wav", "S_UI_Button_Select.wav");

	resolution_value = option_resolution_list_box_->GetCurrentItem();

	// Music Volume Slider
	option_music_volume_slider_ = make_shared<UI_Slider>();
	option_music_volume_slider_->InitSlider("T_Volume_Slider_back.png", "T_Volume_Slider.png", "T_Volume_Slider.png");
	option_window_->AddChildUI("1_Music_VolumeSlider", option_music_volume_slider_);
	option_music_volume_slider_->SetLocalRectByMin({ 40.0f, 410.0f }, 400.0f, 51.0f);
	option_music_volume_slider_->SetSliderLocalRect(44.0f, 18.0f);

	// SFX Volume Slider
	option_sfx_volume_slider_ = make_shared<UI_Slider>();
	option_sfx_volume_slider_->InitSlider("T_Volume_Slider_back.png", "T_Volume_Slider.png", "T_Volume_Slider.png");
	option_window_->AddChildUI("1_SFX_VolumeSlider", option_sfx_volume_slider_);
	option_sfx_volume_slider_->SetLocalRectByMin({ 40.0f, 525.0f }, 400.0f, 51.0f);
	option_sfx_volume_slider_->SetSliderLocalRect(44.0f, 18.0f);

	
}

void UI_OptionWindow::UpdateThisUI()
{
	UIBase::UpdateThisUI();

	if (option_music_volume_slider_->GetIsChanged())
		FMOD_MGR->SetMusicVolume(option_music_volume_slider_->GetValue() / 100.0f);
	if (option_sfx_volume_slider_->GetIsChanged())
		FMOD_MGR->SetSFXVolume(option_sfx_volume_slider_->GetValue() / 100.0f);
}

E_UIState UI_OptionWindow::GetCloseButtonState()
{
	return option_close_button_->GetCurrentState();
}

void UI_OptionWindow::SetCloseButtonState(E_UIState state)
{
	option_close_button_->SetCurrentState(state);
}
