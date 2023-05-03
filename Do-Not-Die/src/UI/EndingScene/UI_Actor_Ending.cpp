#include "UI_Actor_Ending.h"

void UI_Actor_Ending::OnInit(entt::registry& registry)
{
	UIActor::OnInit(registry);

	C_UI& ui_comp = reg_scene_->get<C_UI>(GetEntityId());

	float win_size_1920_width = E_Resolution_Size[E_Resolution::R1920x1080].x;
	float win_size_1920_height = E_Resolution_Size[E_Resolution::R1920x1080].y;

	// Ending Credits
	string ending_credit_tex_id = "T_EndingCredits.png";
	auto tex = RESOURCE->UseResource<Texture>(ending_credit_tex_id);
	ending_credit_size_ = { (int)tex->texture_desc.Width, (int)tex->texture_desc.Height };

	ending_credits_ = make_shared<UI_Image>();
	ending_credits_->InitImage(ending_credit_tex_id);
	ending_credits_->SetLocalRectByMin({ 0.0f, 0.0f }, win_size_1920_width, win_size_1920_height);
	ending_credits_->render_data_.vertex_list[0].t = { 0.0f, 0.0f };
	ending_credits_->render_data_.vertex_list[1].t = { 1.0f, 0.0f };
	ending_credits_->render_data_.vertex_list[2].t = { 0.0f, 1080.0f / ending_credit_size_.y };
	ending_credits_->render_data_.vertex_list[3].t = { 1.0f, 1080.0f / ending_credit_size_.y };
	ending_credits_->UpdateRenderCB();
	ui_comp.ui_list.insert({ "A_EndingCredits", ending_credits_ });

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
	//if (FadeOutBg() == true)
	FadeOutBg();
	timer += TM_DELTATIME;

	ScrollCredit(timer, 30.0f);

	if (timer > 20.0f)
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

void UI_Actor_Ending::ScrollCredit(float cur_timer, float end_time)
{
	float y = ending_credit_size_.y - 1080.0f;
	float top_uv = (y * (cur_timer / end_time)) / ending_credit_size_.y;
	float bottom_uv = (1080.0f + y * (cur_timer / end_time)) / ending_credit_size_.y;

	ending_credits_->render_data_.vertex_list[0].t = { 0.0f, top_uv };
	ending_credits_->render_data_.vertex_list[1].t = { 1.0f, top_uv };
	ending_credits_->render_data_.vertex_list[2].t = { 0.0f, bottom_uv };
	ending_credits_->render_data_.vertex_list[3].t = { 1.0f, bottom_uv };
	ending_credits_->UpdateRenderCB();
}
