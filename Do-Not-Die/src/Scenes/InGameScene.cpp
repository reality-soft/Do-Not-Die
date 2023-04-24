#include "InGameScene.h"
#include "Player.h"
#include "Enemy.h"
#include "FX_Flame.h"
#include "FX_Explosion.h"
#include "FbxMgr.h"

void InGameScene::OnInit()
{
	ShowCursor(false);

	// LOADING : MANAGER_LOADING
	loading_progress = LOADING_MANAGER;

	WRITER->Init();
	reality::ComponentSystem::GetInst()->OnInit(reg_scene_);

	// LOADING : LOADING_SYSTEM
	loading_progress = LOADING_SYSTEM;

	sys_render.OnCreate(reg_scene_);
	sys_camera.OnCreate(reg_scene_);
	sys_ui.OnCreate(reg_scene_);
	sys_camera.SetFarZ(10000.f);
	sys_camera.SetFov(XMConvertToRadians(45));
	sys_camera.mouse_sensivity = 0.5f;
	sys_light.SetGlobalLightPos({ 5000, 5000, -5000 });
	sys_light.OnCreate(reg_scene_);
	sys_effect.OnCreate(reg_scene_);
	sys_sound.OnCreate(reg_scene_);
	
	ingame_ui.OnInit(reg_scene_);

	// LOADING : LOADING_MAP
	loading_progress = LOADING_MAP;

	auto player_entity = Scene::AddPlayer<Player>();
	sys_camera.TargetTag(reg_scene_, "Player");

	auto player_actor = Scene::GetPlayer<Player>(0);
	// Key Settings
	INPUT_EVENT->SubscribeKeyEvent( DIK_D, std::bind(&Player::MoveRight, player_actor), KEY_HOLD);
	INPUT_EVENT->SubscribeKeyEvent( DIK_A , std::bind(&Player::MoveLeft, player_actor), KEY_HOLD);
	INPUT_EVENT->SubscribeKeyEvent( DIK_W, std::bind(&Player::MoveForward, player_actor), KEY_HOLD);
	INPUT_EVENT->SubscribeKeyEvent( DIK_S, std::bind(&Player::MoveBack, player_actor), KEY_HOLD);
	INPUT_EVENT->SubscribeKeyEvent( DIK_RETURN, std::bind(&Player::ResetPos, player_actor), KEY_PUSH);
	INPUT_EVENT->SubscribeKeyEvent( DIK_E, std::bind(&Player::PickClosestItem, player_actor), KEY_PUSH);
								    
	// Select Slot				    
	INPUT_EVENT->SubscribeKeyEvent( DIK_1, std::bind(&Player::SelectSlot, player_actor, 0), KEY_PUSH);
	INPUT_EVENT->SubscribeKeyEvent( DIK_2, std::bind(&Player::SelectSlot, player_actor, 1), KEY_PUSH);
	INPUT_EVENT->SubscribeKeyEvent( DIK_3, std::bind(&Player::SelectSlot, player_actor, 2), KEY_PUSH);
	INPUT_EVENT->SubscribeKeyEvent( DIK_4, std::bind(&Player::SelectSlot, player_actor, 3), KEY_PUSH);
	
	INPUT_EVENT->SubscribeKeyEvent( DIK_Q, std::bind(&Player::UseItem, player_actor), KEY_PUSH);
	INPUT_EVENT->SubscribeKeyEvent( DIK_C, std::bind(&Player::DropItem, player_actor), KEY_PUSH);

	INPUT_EVENT->SubscribeKeyEvent( DIK_SPACE, std::bind(&Player::Jump, player_actor), KEY_PUSH);
	
	INPUT_EVENT->SubscribeMouseEvent( MouseButton::R_BUTTON, std::bind(&Player::Aim, player_actor, true), KEY_PUSH);
	INPUT_EVENT->SubscribeMouseEvent( MouseButton::R_BUTTON, std::bind(&Player::Aim, player_actor, false), KEY_UP);

	std::function<void()> idle = std::bind(&Player::Idle, player_actor);
	INPUT_EVENT->SubscribeKeyEvent( DIK_D, idle, KEY_UP);
	INPUT_EVENT->SubscribeKeyEvent( DIK_S, idle, KEY_UP);
	INPUT_EVENT->SubscribeKeyEvent( DIK_W, idle, KEY_UP);
	INPUT_EVENT->SubscribeKeyEvent( DIK_A, idle, KEY_UP);

	INPUT_EVENT->SubscribeMouseEvent( MouseButton::L_BUTTON , std::bind(&Player::Fire, player_actor), KEY_PUSH);
	INPUT_EVENT->SubscribeKeyEvent( DIK_G , std::bind(&Player::ThrowGrenade, player_actor), KEY_PUSH);
	//INPUT_EVENT->SubscribeMouseEvent({ MouseButton::L_BUTTON }, idle, KEY_UP);

	level.Create("DNDLevel_WithCollision_01.stmesh", "LevelVS.cso");

	//QUADTREE->Init(&level, 3, reg_scene_);
	QUADTREE->ImportGuideLines("DND_Blocking_1.mapdat", GuideType::eBlocking);
	QUADTREE->ImportGuideLines("DND_NpcTrack_1.mapdat", GuideType::eNpcTrack);
	QUADTREE->ImportGuideLines("DND_PlayerStart_1.mapdat", GuideType::eSpawnPoint);
	QUADTREE->ImportGuideLines("DND_ItemSpawn_1.mapdat", GuideType::eSpawnPoint);
	QUADTREE->ImportGuideLines("DND_RepairPart_1.mapdat", GuideType::eSpawnPoint);
	QUADTREE->ImportGuideLines("DND_CarAttack_1.mapdat", GuideType::eNpcTrack);
	QUADTREE->ImportGuideLines("DND_CarEvent_1.mapdat", GuideType::eSpawnPoint);
	QUADTREE->ImportGuideLines("DND_FX_CorpseFire_1.mapdat", GuideType::eSpawnPoint);
	QUADTREE->ImportGuideLines("DND_FX_CarFire_1.mapdat", GuideType::eSpawnPoint);

	QUADTREE->Init(&level, reg_scene_);
	QUADTREE->ImportQuadTreeData("QuadTreeData_01.mapdat");
	QUADTREE->CreatePhysicsCS();
	QUADTREE->InitCollisionMeshes();
	QUADTREE->SetBlockingFields("DND_Blocking_1");

	XMVECTOR plyer_spawn = QUADTREE->GetGuideLines("DND_PlayerStart_1")->begin()->line_nodes.begin()->second;
	player_actor->SetSpawnPoint(plyer_spawn);

	// LOADING : LOADING_ACTOR
	loading_progress = LOADING_ACTOR;
	
	environment_.CreateEnvironment();
	environment_.SetWorldTime(120, 240);
	environment_.SetSkyColorByTime(RGB_TO_FLOAT(201, 205, 204), RGB_TO_FLOAT(11, 11, 19));
	environment_.SetFogDistanceByTime(5000, 1000);
	environment_.SetLightProperty(0.2f, 0.2f);

	// LOADING FINISH
	loading_progress = LOADING_FINISHED;
	
	// Trigger And Wave System
	sys_wave_.OnCreate(reg_scene_);
	sys_wave_.SetWorldEnv(&environment_);
	sys_trigger_.OnCreate(reg_scene_);

	QUADTREE->ImportFloydRout("DND_FloydRout_1.mapdat");


#ifdef _DEBUG
	GUI->AddWidget<PropertyWidget>("property");
	GUI->FindWidget<PropertyWidget>("property")->AddProperty<int>("FPS", &TIMER->fps);
	GUI->FindWidget<PropertyWidget>("property")->AddProperty<float>("Time Countdown", &sys_wave_.countdown_timer_);
	GUI->FindWidget<PropertyWidget>("property")->AddProperty<UINT>("Waves", &sys_wave_.wave_count_);
	GUI->FindWidget<PropertyWidget>("property")->AddProperty<UINT>("RaycastTri", &QUADTREE->raycast_calculated);
	
	GUI->FindWidget<PropertyWidget>("property")->AddProperty<float>("Jump", &player_actor->GetMovementComponent()->jump_pulse);
	GUI->FindWidget<PropertyWidget>("property")->AddProperty<float>("Gravity", &player_actor->GetMovementComponent()->gravity_pulse);
	GUI->FindWidget<PropertyWidget>("property")->AddProperty<UINT>("Selectable Items", &player_actor->selectable_counts_);
	GUI->FindWidget<PropertyWidget>("property")->AddProperty<int>("Car Repaired", &sys_wave_.car_health);
	GUI->FindWidget<PropertyWidget>("property")->AddProperty<int>("Created Actors", &cur_zombie_created);
	GUI->FindWidget<PropertyWidget>("property")->AddProperty<bool>("In Repair Volume", &player_actor->can_repair_car_);
#endif
}

void InGameScene::OnUpdate()
{
	QUADTREE->Frame(&sys_camera);
	QUADTREE->UpdatePhysics();

	sys_camera.OnUpdate(reg_scene_);
	sys_animation.OnUpdate(reg_scene_);
	sys_effect.OnUpdate(reg_scene_);
	sys_light.OnUpdate(reg_scene_);
	sys_movement.OnUpdate(reg_scene_);
	sys_sound.OnUpdate(reg_scene_);

	// Trigger And Wave System
	sys_wave_.OnUpdate(reg_scene_);
	sys_trigger_.OnUpdate(reg_scene_);
	environment_.Update(&sys_camera, &sys_light);
	ingame_ui.SetGameTimer(sys_wave_.countdown_timer_);
	ingame_ui.OnUpdate();
	
	CursorStateUpdate();
}

void InGameScene::OnRender()
{
	environment_.Render();	

	level.Update();
	level.Render();

	QUADTREE->RenderCollisionMeshes();

	sys_render.OnUpdate(reg_scene_);
	sys_ui.OnUpdate(reg_scene_);

#ifdef _DEBUG
	GUI->RenderWidgets();
#endif
}

void InGameScene::OnRelease()
{
	QUADTREE->Release();
	reality::RESOURCE->Release();
}

void InGameScene::CursorStateUpdate()
{
	if (DINPUT->GetKeyState(DIK_T) == KeyState::KEY_PUSH)
	{
		RECT clrect;
		GetWindowRect(ENGINE->GetWindowHandle(), &clrect);

		if (b_show_cursor)
		{
			SetCursorInvisible();
			ClipCursor(&clrect);
		}
		else
		{
			SetCursorVisible();
			ClipCursor(nullptr);
		}
	}
}

void InGameScene::SetCursorVisible()
{
	b_show_cursor = true;
	ShowCursor(b_show_cursor);
}

void InGameScene::SetCursorInvisible()
{
	b_show_cursor = false;
	ShowCursor(b_show_cursor);
	SetCursorPos(ENGINE->GetWindowSize().x / 2.0f, ENGINE->GetWindowSize().y / 2.0f);
}


