#include "PopScene.h"

int WINAPI wWinMain(HINSTANCE hinstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	srand(time(NULL));

	ENGINE->OnInit(hinstance, L"Do Not Die", E_Resolution::RPOP, false);

	SCENE_MGR->AddScene<PopScene>(E_SceneType::POP);
	SCENE_MGR->ChangeScene(E_SceneType::POP);
	ENGINE->Run();

	ENGINE->OnRelease();

	return 0;
} 