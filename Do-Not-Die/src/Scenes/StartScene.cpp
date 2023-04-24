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

	loading_progress = START_ACTOR;
	start_scene_ui.OnInit(reg_scene_);

	loading_progress = START_FINISHED;
}

void StartScene::OnUpdate()
{
	start_scene_ui.OnUpdate();
	sys_sound.OnUpdate(reg_scene_);
}

void StartScene::OnRender()
{
	sys_render.OnUpdate(reg_scene_);
	sys_ui.OnUpdate(reg_scene_);
}

void StartScene::OnRelease()
{
	reality::RESOURCE->Release();
}
