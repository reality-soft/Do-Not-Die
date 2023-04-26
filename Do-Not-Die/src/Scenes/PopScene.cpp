#include "PopScene.h"
#include "StartScene.h"
#include "IntroScene.h"

using namespace reality;

void PopScene::OnInit()
{
	WRITER->Init();

	RESOURCE->GetResourceMutex().lock();
	RESOURCE->LoadDir("../../Contents/Texture/UI/PopScene/", &ResourceMgr::ImportTexture);
	RESOURCE->LoadDir("../../Contents/Shader/", &ResourceMgr::ImportShaders);
	RESOURCE->GetResourceMutex().unlock();

	COMPONENT->OnInit(reg_scene_);

	pop_scene_ui.OnInit(reg_scene_);
	sys_ui.OnCreate(reg_scene_);

	thread loading_thread = thread(&PopScene::LoadingScenes, this);
	loading_thread.detach();
}

void PopScene::OnUpdate()
{
	// When exit Button Selected
	if (pop_scene_ui.exit_button_->GetCurrentState() == E_UIState::UI_SELECT)
	{
		DestroyWindow(ENGINE->GetWindowHandle());
	}

	// 0 : start loading
	// 1 : ingame loading

	static int loading = 0;
	switch (loading)
	{
	case 0:
		if (start_scene == nullptr)
		{
			if (SCENE_MGR->GetScene(START) != NULL)
				start_scene = (StartScene*)SCENE_MGR->GetScene(START).get();
		}
		else
		{
			switch (start_scene->GetLoadingProgress())
			{
			case E_StartLoading::START_START:
				pop_scene_ui.progress_text_->SetText("Start Loading Start Scene");
				break;

			case E_StartLoading::START_RESOURCE:
				pop_scene_ui.progress_text_->SetText("Loading Resources");
				break;

			case E_StartLoading::START_MANAGER:
				pop_scene_ui.progress_text_->SetText("Loading Managers");
				break;

			case E_StartLoading::START_SYSTEM:
				pop_scene_ui.progress_text_->SetText("Loading StartScene System");
				break;

			case E_StartLoading::START_ACTOR:
				pop_scene_ui.progress_text_->SetText("Loading StartScene Actor");
				break;

			case E_StartLoading::START_FINISHED:
				pop_scene_ui.progress_text_->SetText("Loading StartScene Finished");
				loading = 1;
				break;
			}
		}
		break;
	case 1:
		if (ingame_scene == nullptr)
		{
			if (SCENE_MGR->GetScene(INGAME) != NULL)
				ingame_scene = (InGameScene*)SCENE_MGR->GetScene(INGAME).get();
		}
		else
		{
			switch (ingame_scene->GetLoadingProgress())
			{
			case E_IngameLoading::LOADING_START:
				pop_scene_ui.progress_text_->SetText("Start Loading InGameScene");
				break;
			case E_IngameLoading::LOADING_MANAGER:
				pop_scene_ui.progress_text_->SetText("Loading InGameScene Manager");
				break;
			case E_IngameLoading::LOADING_SYSTEM:
				pop_scene_ui.progress_text_->SetText("Loading InGameScene System");
				break;
			case E_IngameLoading::LOADING_MAP:
				pop_scene_ui.progress_text_->SetText("Loading InGameScene Map");
				break;
			case E_IngameLoading::LOADING_ACTOR:
				pop_scene_ui.progress_text_->SetText("Loading InGameScene Actor");
				break;
			case E_IngameLoading::LOADING_FINISHED:
				pop_scene_ui.progress_text_->SetText("Ready to Start");
				if (pop_scene_ui.start_button_->GetCurrentState() == E_UIState::UI_SELECT)
				{
					ENGINE->Resize(E_Resolution::R1920x1080);
					SCENE_MGR->ChangeScene(E_SceneType::INTRO);
				}
				break;
			}
		}
		break;
	}
	
}

void PopScene::OnRender()
{
	sys_ui.OnUpdate(reg_scene_);
}

void PopScene::OnRelease()
{
	//sys_ui.OnRelease();
}

void PopScene::LoadingScenes()
{
	SCENE_MGR->AddScene<StartScene>(START);
	SCENE_MGR->AddScene<IntroScene>(INTRO);
	SCENE_MGR->AddScene<InGameScene>(INGAME);
}