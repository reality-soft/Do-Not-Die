#pragma once
#include "Engine_Include.h"
#include "UI_Actor_StartScene.h"

using namespace reality;

enum E_StartLoading
{
	START_START = 0,
	START_RESOURCE = 1,
	START_MANAGER = 2,
	START_SYSTEM = 3,
	START_ACTOR = 4,

	START_FINISHED = 999,
};

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
private:
	E_StartLoading loading_progress = START_START;
public:
	E_StartLoading GetLoadingProgress() { return loading_progress; }
};

