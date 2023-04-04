#include "LoadingScene.h"
#include "FbxMgr.h"

void LoadingScene::OnInit()
{
	reality::ComponentSystem::GetInst()->OnInit(reg_scene_);
	
	sys_render.OnCreate(reg_scene_);

	loading_scene_ui.OnInit(reg_scene_);
	sys_ui.OnCreate(reg_scene_);
	sys_sound.OnCreate(reg_scene_);
}

void LoadingScene::OnUpdate()
{
	sys_sound.OnUpdate(reg_scene_);
	sys_ui.OnUpdate(reg_scene_);
}

void LoadingScene::OnRender()
{
	sys_render.OnUpdate(reg_scene_);
	sys_ui.OnUpdate(reg_scene_);
}

void LoadingScene::OnRelease()
{
	reality::RESOURCE->Release();
}