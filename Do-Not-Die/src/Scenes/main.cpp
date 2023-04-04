#include "InGameScene.h"
#include "StartScene.h"

int WINAPI wWinMain(HINSTANCE hinstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	ENGINE->OnInit(hinstance, L"Game-Engine", { 1920, 1080 });

	SCENE_MGR->SetScene<StartScene>();
	//SCENE_MGR->SetScene<InGameScene>();
	ENGINE->Run();

	ENGINE->OnRelease();

	return 0;
}