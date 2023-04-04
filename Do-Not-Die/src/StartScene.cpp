#include "StartScene.h"
#include "FbxMgr.h"

void StartScene::OnInit()
{
	reality::RESOURCE->Init("../../Contents/");

	WRITER->Init();
	reality::ComponentSystem::GetInst()->OnInit(reg_scene_);
	
	sys_render.OnCreate(reg_scene_);

	start_scene_ui.OnInit(reg_scene_);
	sys_ui.OnCreate(reg_scene_);
	sys_sound.OnCreate(reg_scene_);
}

void StartScene::OnUpdate()
{
	sys_sound.OnUpdate(reg_scene_);
	sys_ui.OnUpdate(reg_scene_);
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