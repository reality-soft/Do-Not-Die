#include "StartScene.h"
#include "LoadingScene.h"
#include "InGameScene.h"

int WINAPI wWinMain(HINSTANCE hinstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	ENGINE->OnInit(hinstance, L"Game-Engine", { 1920, 1080 });

	SCENE_MGR->AddScene<StartScene>(E_SceneType::START);
	SCENE_MGR->ChangeScene(E_SceneType::START);
	ENGINE->Run();

	ENGINE->OnRelease();

	return 0;
}