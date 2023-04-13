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

	GUI->AddWidget("property", &gw_property_);

	WRITER->Init();
	//reality::ComponentSystem::GetInst()->OnInit(reg_scene_);
	//FbxImportOption option;
	//option.import_scale = 10.0f;
	//option.recalculate_normal = true;
	//FBX->ImportAndSaveFbx("../../Contents/FBX/DNDLevel_WithCollision_01.fbx", option, FbxVertexOption::BY_POLYGON_VERTEX);
	

	// LOADING : LOADING_SYSTEM
	loading_progress = LOADING_SYSTEM;

	sys_render.OnCreate(reg_scene_);
	sys_camera.OnCreate(reg_scene_);
	sys_ui.OnCreate(reg_scene_);
	sys_camera.SetSpeed(1000);
	sys_camera.SetFarZ(10000.f);
	sys_camera.SetFov(XMConvertToRadians(45));
	sys_light.SetGlobalLightPos({ 5000, 5000, -5000 });
	sys_light.OnCreate(reg_scene_);
	sys_effect.OnCreate(reg_scene_);
	sys_sound.OnCreate(reg_scene_);

	ingame_ui.OnInit(reg_scene_);

	// LOADING : LOADING_MAP
	loading_progress = LOADING_MAP;

	auto player_entity = Scene::AddPlayer<Player>();
	sys_camera.TargetTag(reg_scene_, "Player");

	auto character_actor = Scene::GetPlayer<Player>(0);
	// Key Settings
	INPUT_EVENT->SubscribeKeyEvent({ DIK_D }, std::bind(&Player::MoveRight, character_actor), KEY_HOLD);
	INPUT_EVENT->SubscribeKeyEvent({ DIK_W, DIK_D }, std::bind(&Player::MoveRightForward, character_actor), KEY_HOLD);
	INPUT_EVENT->SubscribeKeyEvent({ DIK_S, DIK_D }, std::bind(&Player::MoveRightBack, character_actor), KEY_HOLD);
	INPUT_EVENT->SubscribeKeyEvent({ DIK_A }, std::bind(&Player::MoveLeft, character_actor), KEY_HOLD);
	INPUT_EVENT->SubscribeKeyEvent({ DIK_W, DIK_A }, std::bind(&Player::MoveLeftForward, character_actor), KEY_HOLD);
	INPUT_EVENT->SubscribeKeyEvent({ DIK_S, DIK_A }, std::bind(&Player::MoveLeftBack, character_actor), KEY_HOLD);
	INPUT_EVENT->SubscribeKeyEvent({ DIK_W }, std::bind(&Player::MoveForward, character_actor), KEY_HOLD);
	INPUT_EVENT->SubscribeKeyEvent({ DIK_S }, std::bind(&Player::MoveBack, character_actor), KEY_HOLD);
	INPUT_EVENT->SubscribeKeyEvent({ DIK_RETURN }, std::bind(&Player::ResetPos, character_actor), KEY_PUSH);

	// Item Use
	INPUT_EVENT->SubscribeKeyEvent({ DIK_1 }, std::bind(&Player::UseItem, character_actor, 0), KEY_PUSH);
	INPUT_EVENT->SubscribeKeyEvent({ DIK_2 }, std::bind(&Player::UseItem, character_actor, 1), KEY_PUSH);
	INPUT_EVENT->SubscribeKeyEvent({ DIK_3 }, std::bind(&Player::UseItem, character_actor, 2), KEY_PUSH);
	INPUT_EVENT->SubscribeKeyEvent({ DIK_4 }, std::bind(&Player::UseItem, character_actor, 3), KEY_PUSH);

	INPUT_EVENT->SubscribeKeyEvent({ DIK_SPACE }, std::bind(&Player::Jump, character_actor), KEY_PUSH);
	
	INPUT_EVENT->SubscribeKeyEvent({ DIK_Q }, std::bind(&Player::Aim, character_actor), KEY_PUSH);

	std::function<void()> idle = std::bind(&Player::Idle, character_actor);
	INPUT_EVENT->SubscribeKeyEvent({ DIK_D }, idle, KEY_UP);
	INPUT_EVENT->SubscribeKeyEvent({ DIK_S }, idle, KEY_UP);
	INPUT_EVENT->SubscribeKeyEvent({ DIK_W }, idle, KEY_UP);
	INPUT_EVENT->SubscribeKeyEvent({ DIK_A }, idle, KEY_UP);

	INPUT_EVENT->SubscribeMouseEvent({ MouseButton::L_BUTTON }, std::bind(&Player::Fire, character_actor), KEY_HOLD);
	//INPUT_EVENT->SubscribeMouseEvent({ MouseButton::L_BUTTON }, idle, KEY_UP);

	level.Create("DNDLevel_WithCollision_01.stmesh", "LevelVS.cso");
	//level.ImportGuideLines("../../Contents/BinaryPackage/DeadPoly_Blocking1.mapdat", GuideLine::GuideType::eBlocking);
	level.ImportGuideLines("../../Contents/BinaryPackage/DeadPoly_NpcTrack_01.mapdat", GuideLine::GuideType::eNpcTrack);

	//QUADTREE->Init(&level, 3, reg_scene_);
	QUADTREE->Init(&level, reg_scene_);
	QUADTREE->ImportQuadTreeData("../../Contents/BinaryPackage/QuadTreeData_01.matdat");
	QUADTREE->CreatePhysicsCS();
	QUADTREE->InitCollisionMeshes();
	
	// LOADING : LOADING_ACTOR
	loading_progress = LOADING_ACTOR;

	environment_.CreateEnvironment();
	environment_.SetWorldTime(60, 60, true);
	environment_.SetSkyColorByTime(RGB_TO_FLOAT(201, 205, 204), RGB_TO_FLOAT(11, 11, 19));
	environment_.SetFogDistanceByTime(5000, 1000);
	environment_.SetLightProperty(0.2f, 0.2f);

	gw_property_.AddProperty<float>("FPS", &TIMER->fps);
	gw_property_.AddProperty<UINT>("visible nodes", &QUADTREE->visible_nodes);

	EFFECT_MGR->SpawnEffect<FX_Flame>(E_SceneType::INGAME, XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f), XMQuaternionIdentity(), XMVectorSet(10.0f, 10.0f, 10.0f, 0.0f));


	// LOADING FINISH
	loading_progress = LOADING_FINISHED;
}

void InGameScene::OnUpdate()
{
	static float cur_time = 0.0f;

	cur_time += TM_DELTATIME;

	const vector<reality::GuideLine> npc_guidlines = level.GetGuideLines(reality::GuideLine::GuideType::eNpcTrack);

	if (cur_time >= 10.0f) {
		auto enemy_entity = SCENE_MGR->AddActor<Enemy>();
		auto enemy_actor = SCENE_MGR->GetActor<Enemy>(enemy_entity);

		int guidline_index = rand() % npc_guidlines.size();
		int mesh_index = rand() % enemy_meshes.size();

		vector<XMVECTOR> target_poses;
		for (const auto& target_pos : npc_guidlines[guidline_index].line_nodes) {
			target_poses.push_back(target_pos.second);
		}
		enemy_actor->SetRoute(target_poses);
		enemy_actor->SetMeshId(enemy_meshes[mesh_index]);
		
		//auto player = SCENE_MGR->GetPlayer<Player>(0);
		//player->SetPos(level.GetGuideLines()->at(guidline_index).line_nodes[0]);
		
		cur_time = 0.0f;

		cur_zombie_created++;
	}


	sys_camera.OnUpdate(reg_scene_);
	sys_animation.OnUpdate(reg_scene_);
	sys_light.OnUpdate(reg_scene_);
	sys_movement.OnUpdate(reg_scene_);
	sys_effect.OnUpdate(reg_scene_);
	sys_sound.OnUpdate(reg_scene_);
	QUADTREE->Frame(&sys_camera);
	QUADTREE->UpdatePhysics("PhysicsCS.cso");

	environment_.Update(&sys_camera, &sys_light);

	ingame_ui.OnUpdate();

	if (DINPUT->GetKeyState(DIK_G) == KeyState::KEY_PUSH)
		CreateExplosionEffectFromRay();

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

	GUI->RenderWidgets();
}

void InGameScene::OnRelease()
{
	QUADTREE->Release();
	reality::RESOURCE->Release();
}

void InGameScene::CreateExplosionEffectFromRay()
{
	RayShape ray = sys_camera.CreateFrontRay();

	RayCallback raycallback = QUADTREE->Raycast(ray);

	if (raycallback.success)
	{
		EFFECT_MGR->SpawnEffect<FX_Explosion>(raycallback.point);		
	}
}

void InGameScene::CursorStateUpdate()
{
	if (DINPUT->GetKeyState(DIK_T) == KeyState::KEY_PUSH)
	{
		if (b_show_cursor)
			SetCursorInvisible();
		else
			SetCursorVisible();
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


