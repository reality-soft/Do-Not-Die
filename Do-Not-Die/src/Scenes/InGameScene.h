#pragma once
#include "Engine_Include.h"
#include "TestWidget.h"
#include "UI_Actor_Ingame.h"
#include "StaticShadowMap.h"

using namespace reality;

enum E_IngameLoading
{
	LOADING_START = 0,
	LOADING_MANAGER = 1,
	LOADING_SYSTEM = 2,
	LOADING_MAP = 3,
	LOADING_ACTOR = 4,

	LOADING_FINISHED = 999,
};

class InGameScene : public reality::Scene
{
public:
	virtual void OnInit();
	virtual void OnUpdate();
	virtual void OnRender();
	virtual void OnRelease();

private:
	vector<std::string> enemy_meshes = {
		"Zombie_Businessman_Male_01.skmesh",
		"Zombie_Cheerleader_Female_01.skmesh",
		"Zombie_Daughter_Female_01.skmesh",
		"Zombie_Police_Male_01.skmesh",
		"Zombie_RiotCop_Male_01.skmesh",
		"Zombie_SchoolGirl_Female_01.skmesh",
		"Zombie_Bellboy_Male_01.skmesh",
	};

	Environment environment_;
	StaticShadowMap ssm;
	StaticMeshLevel level;

	reality::LightingSystem sys_light;
	reality::RenderSystem sys_render;
	reality::CameraSystem sys_camera;
	reality::SoundSystem sys_sound;
	reality::EffectSystem sys_effect;
	reality::MovementSystem  sys_movement;
	reality::UISystem		sys_ui;

	StaticObject test_object;

private:
	TestWidget	test_window_;
	PropertyWidget gw_property_;
	UI_Actor_Ingame ingame_ui;
	void CreateImpactEffectFromRay();
	void CreateExplosionEffectFromRay();
	void CursorStateUpdate();

private:
	int cur_zombie_created = 0;
private:
	bool b_show_cursor = false;
public:
	void SetCursorVisible();
	void SetCursorInvisible();
private:
	E_IngameLoading loading_progress = LOADING_START;
public:
	E_IngameLoading GetLoadingProgress() { return loading_progress; }
};

