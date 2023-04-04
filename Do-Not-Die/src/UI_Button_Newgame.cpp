#include "stdafx.h"
#include "UI_Button_Newgame.h"
#include "SceneMgr.h"
#include "TestGame.h"

using namespace reality;

void UI_Button_Newgame::Update()
{
	UI_Button::Update();

	if (current_state_ == E_UIState::UI_SELECT)
		SCENE_MGR->SetScene<TestGame>();
}
