#include "StartScene.h"
#include "FbxMgr.h"

void StartScene::OnInit()
{
	loading_progress = START_RESOURCE;
	reality::RESOURCE->Init("../../Contents/");

	loading_progress = START_MANAGER;
	COMPONENT->OnInit(reg_scene_);

	loading_progress = START_SYSTEM;
	sys_render.OnCreate(reg_scene_);
	sys_ui.OnCreate(reg_scene_);
	sys_sound.OnCreate(reg_scene_);
	sys_effect.OnCreate(reg_scene_);
	sys_light.OnCreate(reg_scene_);
	sys_light.SetGlobalLightPos({ 5000, 5000, -5000 });

	loading_progress = START_ACTOR;

	sequence_camera_ = Scene::AddActor<SequenceCameraActor>();
	sequence_camera_actor = Scene::GetActor<SequenceCameraActor>(sequence_camera_);
	sequence_camera_actor->ImportSequenceTrack("DND_SequenceTrack_1.mapdat");
	start_scene_ui.OnInit(reg_scene_);

	level.Create("DNDLevel_WithCollision_01.stmesh", "LevelVS.cso");

	environment_.CreateEnvironment();
	environment_.SetWorldTime(120, 120);
	environment_.SetSkyColorByTime(RGB_TO_FLOAT(201, 205, 204), RGB_TO_FLOAT(11, 11, 19));
	environment_.SetFogDistanceByTime(5000, 1000);
	environment_.SetLightProperty(0.2f, 0.2f);

	loading_progress = START_FINISHED;
}

void StartScene::OnUpdate()
{
	sys_effect.OnUpdate(reg_scene_);
	sys_light.OnUpdate(reg_scene_);
	start_scene_ui.OnUpdate();
	sys_sound.OnUpdate(reg_scene_);
	environment_.Update(sequence_camera_actor->world_pos_, &sys_light);
}

void StartScene::OnRender()
{
	environment_.Render();

	level.Update();
	level.Render();

	sys_render.OnUpdate(reg_scene_);
	sys_ui.OnUpdate(reg_scene_);
}

void StartScene::OnRelease()
{
	reality::RESOURCE->Release();
}
