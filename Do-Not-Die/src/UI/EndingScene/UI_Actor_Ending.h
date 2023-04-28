#pragma once
#include "Engine_include.h"
#include "UIActor.h"

using namespace reality;

class UI_Actor_Ending : public UIActor
{
public:
	virtual void OnInit(entt::registry& registry);
	virtual void OnUpdate();

	// Game Over UI
	shared_ptr<UI_Image>	game_clear_ui_;
	shared_ptr<UI_Image>	game_clear_img_;
	shared_ptr<UI_Button> game_clear_exit_button_;
};

