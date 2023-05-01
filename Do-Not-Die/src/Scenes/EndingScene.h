#pragma once
#include "Engine_include.h"
#include "SequenceCameraActor.h"
#include "RidingCar.h"
#include "UI_Actor_Ending.h"

using namespace reality;

enum E_EndingLoading
{
	ENDING_START = 0,
	ENDING_RESOURCE = 1,
	ENDING_MANAGER = 2,
	ENDING_SYSTEM = 3,
	ENDING_ACTOR = 4,
	ENDING_FINISHED = 999,
};

class EndingScene : public Scene
{
public:
	virtual void OnInit();
	virtual void OnUpdate();
	virtual void OnRender();
	virtual void OnRelease();


	bool scene_finished = false;
	void StartingProgress();
	void FinishProgress();

private:
	RidingCar* riding_car_actor = nullptr;
	SequenceCameraActor* sequence_camera_actor = nullptr;

	Environment environment_;
	StaticMeshLevel level;

	reality::AnimationSystem sys_animation;
	reality::LightingSystem sys_light;
	reality::EffectSystem   sys_effect;
	reality::RenderSystem	sys_render;
	reality::SoundSystem	sys_sound;
	reality::UISystem		sys_ui;

	UI_Actor_Ending ending_ui_;

private:
	E_EndingLoading loading_progress = ENDING_START;
public:
	E_EndingLoading GetLoadingProgress() { return loading_progress; }
};

