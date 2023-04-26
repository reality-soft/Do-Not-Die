#pragma once
#include "Engine_Include.h"
#include "UI_Actor_IntroScene.h"

using namespace reality;

class IntroScene : public reality::Scene
{
public:
	virtual void OnInit();
	virtual void OnUpdate();
	virtual void OnRender();
	virtual void OnRelease();
private:
	reality::SoundSystem	sys_sound;
	reality::UISystem		sys_ui;
private:
	UI_Actor_IntroScene intro_scene_ui;
};

