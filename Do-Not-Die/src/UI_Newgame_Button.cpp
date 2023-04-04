#include "stdafx.h"
#include "UI_Newgame_Button.h"
#include "SceneMgr.h"
#include "TestGame.h"

using namespace reality;

void UI_Newgame_Button::Update()
{
	UI_Button::Update();

	if (current_state_ == E_UIState::UI_SELECT)
		SCENE_MGR->SetScene<TestGame>();
}
