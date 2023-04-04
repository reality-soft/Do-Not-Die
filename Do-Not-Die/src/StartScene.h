#pragma once
#include "Engine_Include.h"
#include "UI_Actor_StartScene.h"

using namespace reality;

class StartScene : public reality::Scene
{
public:
	virtual void OnInit();
	virtual void OnUpdate();
	virtual void OnRender();
	virtual void OnRelease();

private:
	reality::RenderSystem	sys_render;
	reality::SoundSystem	sys_sound;
	reality::UISystem		sys_ui;

private:
	UI_Actor_StartScene start_scene_ui;
};

