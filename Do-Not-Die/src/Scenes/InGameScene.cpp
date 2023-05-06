#include "InGameScene.h"
#include "Player.h"
#include "NormalZombie.h"
#include "BossZombie.h"
#include "FX_Flame.h"
#include "FX_Explosion.h"
#include "FbxMgr.h"

void InGameScene::OnInit()
{
	// LOADING : MANAGER_LOADING
	loading_progress = LOADING_MANAGER;

	reality::ComponentSystem::GetInst()->OnInit(reg_scene_);

	// LOADING : LOADING_SYSTEM
	loading_progress = LOADING_SYSTEM;

	sys_render.OnCreate(reg_scene_);
	sys_camera.OnCreate(reg_scene_);
	sys_ui.OnCreate(reg_scene_);
	sys_camera.SetFarZ(10000.f);
	sys_camera.SetFov(XMConvertToRadians(45));
	sys_camera.mouse_sensivity = 0.5f;
	sys_light.SetGlobalLightPos({ 5000, 5000, -5000 , 0 });
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
	INPUT_EVENT->SubscribeKeyEvent(DIK_R, std::bind(&Player::Reload, player_actor), KEY_PUSH);
	INPUT_EVENT->SubscribeKeyEvent(DIK_LSHIFT, std::bind(&Player::Roll, player_actor), KEY_PUSH);

	INPUT_EVENT->SubscribeMouseEvent( MouseButton::R_BUTTON, std::bind(&Player::Aim, player_actor, true), KEY_PUSH);
	INPUT_EVENT->SubscribeMouseEvent( MouseButton::R_BUTTON, std::bind(&Player::Aim, player_actor, false), KEY_UP);
	INPUT_EVENT->SubscribeMouseEvent( MouseButton::L_BUTTON , std::bind(&Player::Attack, player_actor), KEY_PUSH);

	level.Create("DNDLevel_WithCollision_01.stmesh", "LevelVS.cso");

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
	QUADTREE->view_collisions_ = true;

	XMVECTOR plyer_spawn = QUADTREE->GetGuideLines("DND_PlayerStart_1")->begin()->line_nodes.begin()->second;
	player_actor->SetSpawnPoint(plyer_spawn);

	// LOADING : LOADING_ACTOR
	loading_progress = LOADING_ACTOR;
	
	environment_.CreateEnvironment();
	//environment_.SetWorldTime(120, 240);
	environment_.SetWorldTime(120, 30);
	environment_.SetSkyColorByTime(RGB_TO_FLOAT(201, 205, 204), RGB_TO_FLOAT(11, 11, 19));
	environment_.SetFogDistanceByTime(5000, 1000);
	environment_.SetLightProperty(XMFLOAT4(1.0, 0.7, 0.5, 1), XMFLOAT4(0.05, 0.05, 0.1, 1), 0.1f, 0.25f);

	// LOADING FINISH
	loading_progress = LOADING_FINISHED;
	
	// Trigger And Wave System
	sys_wave_.OnCreate(reg_scene_);
	sys_wave_.SetWorldEnv(&environment_);
	sys_wave_.RandomSpawnItem(30);

	sys_trigger_.OnCreate(reg_scene_);

#ifdef _DEBUG
	GUI->AddWidget<PropertyWidget>("property");
	GUI->FindWidget<PropertyWidget>("property")->AddProperty<int>("FPS", &TIMER->fps);
	GUI->FindWidget<PropertyWidget>("property")->AddProperty<float>("speed buff timer", &player_actor->GetStatus("max_speed")->timer_);
	GUI->FindWidget<PropertyWidget>("property")->AddProperty<float>("damage buff timer", &player_actor->GetStatus("gunfire_damage")->timer_);
#endif

	//BossZombie Test
	//BossZombie* boss = GetActor<BossZombie>(AddActor<BossZombie>());
	//if (boss)
	//	boss->ApplyMovement(player_actor->GetCurPosition() + XMVectorSet(0, -100, 0, 0));
}

void InGameScene::OnUpdate()
{
	if (DINPUT->GetKeyState(DIK_TAB) == KEY_PUSH)
	{
		sys_wave_.wave_count_++;
	}

	if (game_result_type != GameResultType::eNone)
	{
		GameResultProcess();
	}
	
	QUADTREE->Frame(&sys_camera);
	QUADTREE->UpdatePhysics();

	sys_camera.OnUpdate(reg_scene_);
	environment_.Update(sys_camera.GetCamera()->camera_pos, &sys_light);
	sys_animation.OnUpdate(reg_scene_);
	sys_effect.OnUpdate(reg_scene_);
	sys_light.OnUpdate(reg_scene_);
	sys_movement.OnUpdate(reg_scene_);
	sys_sound.OnUpdate(reg_scene_);

	// Trigger And Wave System
	sys_wave_.OnUpdate(reg_scene_);
	sys_trigger_.OnUpdate(reg_scene_);
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
	while (ShowCursor(b_show_cursor) <= 0);
}

void InGameScene::SetCursorInvisible()
{
	b_show_cursor = false;
	while (ShowCursor(b_show_cursor) >= 0);
	SetCursorPos(ENGINE->GetWindowSize().x / 2.0f, ENGINE->GetWindowSize().y / 2.0f);
}

void InGameScene::ShowCarCrashing()
{
	SequenceInfo seq_info;
	seq_info.sequence_start = sys_camera.world_matrix.r[3];
	seq_info.sequence_end = sys_wave_.GetCarPosition() - XMVector3Normalize(sys_wave_.GetCarPosition() - seq_info.sequence_start) * 500;
	seq_info.target_start = sys_camera.GetCamera()->target_pos;
	seq_info.target_end = sys_wave_.GetCarPosition();
	seq_info.play_time = 5.0f;
	seq_info.acceler = 1.f;
	seq_info.decceler = 0.1f;

	if (sys_camera.PlaySequence(seq_info, -1) == true)
	{
		ingame_ui.ShowCarCrashed();

		static float time = 0.0f;
		static int index = 0;
		time += TM_DELTATIME;
		
		if (time > 0.3 && index < sys_wave_.fx_car_fire_.line_nodes.size())
		{
			XMVECTOR spawn_pos = sys_wave_.fx_car_fire_.line_nodes.at(index++);
			EFFECT_MGR->SpawnEffect<FX_Explosion>(spawn_pos);
			time = 0.0f;
		}			
	}
}

void InGameScene::GameResultProcess()
{
	switch (game_result_type)
	{
	case GameResultType::ePlayerDead:
		ingame_ui.ShowPlayerDead();
		break;
	case GameResultType::eCarCrashed:
		ShowCarCrashing();
		break;
	case GameResultType::ePlayerInfected:
		break;
	case GameResultType::eGameCleared:
		bool fade_out_finished = ingame_ui.FadeOut();
		if (fade_out_finished)
			SCENE_MGR->ChangeScene(ENDING);
		break;
	}	
}

