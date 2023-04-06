#include "LoadingScene.h"
#include "FbxMgr.h"

using namespace reality;

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

	if (ingame_scene == nullptr)
	{
		if(SCENE_MGR->GetScene(INGAME) != NULL)
			ingame_scene = (InGameScene*)SCENE_MGR->GetScene(INGAME).get();
	}
	else
	{
		switch (ingame_scene->GetLoadingProgress())
		{
		case E_IngameLoading::LOADING_START:
			loading_scene_ui.progress_bar_->SetLocalRectByMin(loading_scene_ui.progress_bar_->rect_transform_.local_rect.min, 100.0f, loading_scene_ui.progress_bar_->rect_transform_.local_rect.height);
			break; 
		case E_IngameLoading::LOADING_MANAGER:
			loading_scene_ui.progress_bar_->SetLocalRectByMin(loading_scene_ui.progress_bar_->rect_transform_.local_rect.min, 200.0f, loading_scene_ui.progress_bar_->rect_transform_.local_rect.height);
			break; 
		case E_IngameLoading::LOADING_SYSTEM:
			loading_scene_ui.progress_bar_->SetLocalRectByMin(loading_scene_ui.progress_bar_->rect_transform_.local_rect.min, 400.0f, loading_scene_ui.progress_bar_->rect_transform_.local_rect.height);
			break;
		case E_IngameLoading::LOADING_MAP:
			loading_scene_ui.progress_bar_->SetLocalRectByMin(loading_scene_ui.progress_bar_->rect_transform_.local_rect.min, 600.0f, loading_scene_ui.progress_bar_->rect_transform_.local_rect.height);
			break;
		case E_IngameLoading::LOADING_ACTOR:
			loading_scene_ui.progress_bar_->SetLocalRectByMin(loading_scene_ui.progress_bar_->rect_transform_.local_rect.min, 800.0f, loading_scene_ui.progress_bar_->rect_transform_.local_rect.height);
			break;
		case E_IngameLoading::LOADING_FINISHED:
			loading_scene_ui.progress_bar_->SetLocalRectByMin(loading_scene_ui.progress_bar_->rect_transform_.local_rect.min, 1000.0f, loading_scene_ui.progress_bar_->rect_transform_.local_rect.height);
			SCENE_MGR->ChangeScene(INGAME);
			ShowCursor(false);
			break;
		}
	}
}

void LoadingScene::OnRender()
{
	sys_render.OnUpdate(reg_scene_);
	sys_ui.OnUpdate(reg_scene_);
}

void LoadingScene::OnRelease()
{
}
