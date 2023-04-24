#pragma once
#include "Engine_Include.h"
#include "UI_Actor_PopScene.h"
#include "StartScene.h"
#include "InGameScene.h"

using namespace reality;

class PopScene : public reality::Scene
{
public:
	virtual void OnInit();
	virtual void OnUpdate();
	virtual void OnRender();
	virtual void OnRelease();
private:
	void LoadingScenes();
private:
	reality::UISystem		sys_ui;
private:
	StartScene*		start_scene;
	InGameScene*	ingame_scene;
private:
	UI_Actor_PopScene pop_scene_ui;
};

