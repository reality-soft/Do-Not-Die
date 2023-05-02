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
	void ScrollCredit(float cur_timer, float end_time);

	// Game Over UI
	XMINT2 ending_credit_size_;
	shared_ptr<UI_Image> ending_credits_;
	shared_ptr<UI_Image> ui_fade_background_;
	bool ending_finished = false;
};

