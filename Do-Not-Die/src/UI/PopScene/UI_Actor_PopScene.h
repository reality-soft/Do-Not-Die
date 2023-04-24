#pragma once
#include "UIActor.h"

namespace reality
{
	class UI_Actor_PopScene : public UIActor
	{
	public:
		virtual void OnInit(entt::registry& registry);
		virtual void OnUpdate();
	private:
		shared_ptr<UI_Image>	background_;
	public:
		shared_ptr<UI_Text>		progress_text_;
		shared_ptr<UI_Button>	start_button_;
		shared_ptr<UI_Button>	exit_button_;
	private:
		void CreateUI();
		void UpdateUI();
	};

}

