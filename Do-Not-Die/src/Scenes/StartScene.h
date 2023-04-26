#pragma once
#include "Engine_Include.h"
#include "UI_Actor_Title.h"
#include "UI_Actor_Intro.h"
#include "SequenceCameraActor.h"
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

	bool scene_finished = false;
	void FinishProgress();
private:
	Environment environment_;
	StaticMeshLevel level;
	entt::entity sequence_camera_;
	SequenceCameraActor* sequence_camera_actor = nullptr;
	GuideLine fire_effects_;

	reality::LightingSystem sys_light;
	reality::EffectSystem   sys_effect;
	reality::RenderSystem	sys_render;
	reality::SoundSystem	sys_sound;
	reality::UISystem		sys_ui;
private:
	UI_Actor_Intro intro_scene_ui;
	UI_Actor_Title start_scene_ui; 
private:
	E_StartLoading loading_progress = START_START;
public:
	E_StartLoading GetLoadingProgress() { return loading_progress; }
};

