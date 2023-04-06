#pragma once
#include "UI_Image.h"
#include "UI_Button.h"

namespace reality
{
	class UI_OptionWindow : public UI_Image
	{
	public:
		void InitOptionWindow();
	private:
		shared_ptr<UI_Image>	option_window_;
		shared_ptr<UI_Button>	option_close_button_;
	public:
		E_UIState	GetCloseButtonState();
		void		SetCloseButtonState(E_UIState state);
	};

}

