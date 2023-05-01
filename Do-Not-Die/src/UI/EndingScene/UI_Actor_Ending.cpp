#include "UI_Actor_Ending.h"

void UI_Actor_Ending::OnInit(entt::registry& registry)
{
	UIActor::OnInit(registry);

	C_UI& ui_comp = reg_scene_->get<C_UI>(GetEntityId());

	float win_size_1920_width = E_Resolution_Size[E_Resolution::R1920x1080].x;
	float win_size_1920_height = E_Resolution_Size[E_Resolution::R1920x1080].y;

	// Ending Credits



	// Fade BG
	ui_fade_background_ = make_shared<UI_Image>();
	ui_fade_background_->InitImage("T_Intro_Background.png");
	ui_fade_background_->SetLocalRectByMin({ 0.0f, 0.0f }, win_size_1920_width, win_size_1920_height);
	ui_comp.ui_list.insert({ "Z_FadeBg", ui_fade_background_ });
}

void UI_Actor_Ending::OnUpdate()
{
	ShowCursor(true);

	static float timer = 0;

	if (FadeOutBg() == true)
		timer += TM_DELTATIME;

	if (timer > 15.0f)
		ending_finished = true;
}


bool UI_Actor_Ending::FadeOutBg()
{
	static float timer = 0.0f;
	static float alpha = 1.0f;

	if (alpha < 0.001f)
		return true;

	timer += TM_DELTATIME;
	alpha = FadeOutAlpha(0.0f, 5.0f, timer);

	ui_fade_background_->SetAlpha(alpha);
	return false;
}
bool UI_Actor_Ending::FadeInBg()
{
	static float timer = 0.0f;
	static float alpha = 0.0f;

	if (alpha > 0.999f)
		return true;

	timer += TM_DELTATIME;
	alpha = FadeInAlpha(0.0f, 5.0f, timer);

	ui_fade_background_->SetAlpha(alpha);
	return false;
}
