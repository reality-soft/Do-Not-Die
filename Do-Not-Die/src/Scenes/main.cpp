#include "StartScene.h"
#include "LoadingScene.h"
#include "InGameScene.h"

int WINAPI wWinMain(HINSTANCE hinstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	ENGINE->OnInit(hinstance, L"Game-Engine", E_Resolution::R1920x1080, false);

	SCENE_MGR->AddScene<StartScene>(E_SceneType::START);
	SCENE_MGR->ChangeScene(E_SceneType::START);
	ENGINE->Run();

	ENGINE->OnRelease();

	return 0;
} 