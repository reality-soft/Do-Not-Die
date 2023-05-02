#pragma once
#include "Engine_include.h"
#include "UIActor.h"

using namespace reality;

class UI_Actor_Ending : public UIActor
{
public:
	virtual void OnInit(entt::registry& registry);
	virtual void OnUpdate();

	bool FadeOutBg();
	bool FadeInBg();

	// Game Over UI
	shared_ptr<UI_Image> ui_fade_background_;
	bool ending_finished = false;
};

