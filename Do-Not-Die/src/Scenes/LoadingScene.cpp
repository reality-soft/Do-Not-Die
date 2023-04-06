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

	static float init_width = loading_scene_ui.progress_bar_->rect_transform_.local_rect.width;
	static float percentage = 0;
	static float speed = 10.0f;

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
			if (percentage < 10.0f)
				percentage += speed * TIMER->GetDeltaTime();
			loading_scene_ui.progress_bar_->SetLocalRectByMin(loading_scene_ui.progress_bar_->rect_transform_.local_rect.min, 
				init_width * percentage / 100.0f, loading_scene_ui.progress_bar_->rect_transform_.local_rect.height);
			break; 
		case E_IngameLoading::LOADING_MANAGER:
			if (percentage < 20.0f)
				percentage += speed * TIMER->GetDeltaTime();
			loading_scene_ui.progress_bar_->SetLocalRectByMin(loading_scene_ui.progress_bar_->rect_transform_.local_rect.min, init_width * percentage / 100.0f, 
				loading_scene_ui.progress_bar_->rect_transform_.local_rect.height);
			break; 
		case E_IngameLoading::LOADING_SYSTEM:
			if (percentage < 40.0f)
				percentage += speed * TIMER->GetDeltaTime();
			loading_scene_ui.progress_bar_->SetLocalRectByMin(loading_scene_ui.progress_bar_->rect_transform_.local_rect.min, init_width * percentage / 100.0f, 
				loading_scene_ui.progress_bar_->rect_transform_.local_rect.height);
			break;
		case E_IngameLoading::LOADING_MAP:
			if (percentage < 80.0f)
				percentage += speed * TIMER->GetDeltaTime();
			loading_scene_ui.progress_bar_->SetLocalRectByMin(loading_scene_ui.progress_bar_->rect_transform_.local_rect.min, init_width * percentage / 100.0f, 
				loading_scene_ui.progress_bar_->rect_transform_.local_rect.height);
			break;
		case E_IngameLoading::LOADING_ACTOR:
			if (percentage < 90.0f)
				percentage += speed * TIMER->GetDeltaTime();
			loading_scene_ui.progress_bar_->SetLocalRectByMin(loading_scene_ui.progress_bar_->rect_transform_.local_rect.min, init_width * percentage / 100.0f, 
				loading_scene_ui.progress_bar_->rect_transform_.local_rect.height);
			break;
		case E_IngameLoading::LOADING_FINISHED:
			if (percentage <= 100.0f)
				percentage += speed * TIMER->GetDeltaTime();
			loading_scene_ui.progress_bar_->SetLocalRectByMin(loading_scene_ui.progress_bar_->rect_transform_.local_rect.min, init_width * percentage / 100.0f, 
				loading_scene_ui.progress_bar_->rect_transform_.local_rect.height);
			if (percentage > 100.0f)
			{
				Sleep(200);
				SCENE_MGR->ChangeScene(INGAME);
				ShowCursor(false);
			}
			
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
