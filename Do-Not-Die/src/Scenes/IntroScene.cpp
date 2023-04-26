#include "IntroScene.h"
#include "FbxMgr.h"

using namespace reality;

void IntroScene::OnInit()
{
	reality::ComponentSystem::GetInst()->OnInit(reg_scene_);
	

	intro_scene_ui.OnInit(reg_scene_);
	intro_scene_ui.SetIntroTime(6.0f);


	sys_ui.OnCreate(reg_scene_);
	sys_sound.OnCreate(reg_scene_);
}

void IntroScene::OnUpdate()
{
	sys_sound.OnUpdate(reg_scene_);
	intro_scene_ui.OnUpdate();

	if (intro_scene_ui.GetTimer() > intro_scene_ui.GetIntroTime())
		SCENE_MGR->ChangeScene(E_SceneType::START);
}

void IntroScene::OnRender()
{
	sys_ui.OnUpdate(reg_scene_);
}

void IntroScene::OnRelease()
{
}
