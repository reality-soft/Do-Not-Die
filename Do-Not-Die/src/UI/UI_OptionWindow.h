#pragma once
#include "UI_Image.h"
#include "UI_Button.h"
#include "UI_Slider.h"
#include "UI_Listbox.h"

namespace reality
{
	class UI_OptionWindow : public UI_Image
	{
	public:
		void InitOptionWindow();
	protected:
		void UpdateThisUI() override;
	private:
		shared_ptr<UI_Image>	option_window_;
		shared_ptr<UI_Button>	option_close_button_;
		shared_ptr<UI_Listbox>  option_resolution_list_box_;
		shared_ptr<UI_Slider>	option_volume_slider_;
	public:
		E_UIState	GetCloseButtonState();
		void		SetCloseButtonState(E_UIState state);
	};

}

