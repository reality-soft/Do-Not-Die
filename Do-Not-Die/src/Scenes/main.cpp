#include "PopScene.h"

int WINAPI wWinMain(HINSTANCE hinstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	ENGINE->OnInit(hinstance, L"Game-Engine", E_Resolution::RPOP, false);

	SCENE_MGR->AddScene<PopScene>(E_SceneType::POP);
	SCENE_MGR->ChangeScene(E_SceneType::POP);
	ENGINE->Run();

	ENGINE->OnRelease();

	return 0;
} 