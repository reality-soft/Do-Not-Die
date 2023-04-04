#include "InGameScene.h"
#include "Player.h"
#include "Enemy.h"
#include "FX_BloodImpact.h"
#include "FX_ConcreteImpact.h"
#include "FX_Flame.h"
#include "FX_Muzzle.h"
#include "FX_Explosion.h"
#include "FbxMgr.h"

void InGameScene::OnInit()
{
	ShowCursor(false);
	//SetCapture(ENGINE->GetWindowHandle());

	GUI->AddWidget("property", &gw_property_);

	reality::RESOURCE->Init("../../Contents/");

	WRITER->Init();
	reality::ComponentSystem::GetInst()->OnInit(reg_scene_);
	
	sys_render.OnCreate(reg_scene_);
	sys_camera.OnCreate(reg_scene_);

	ingame_ui.OnInit(reg_scene_);
	sys_ui.OnCreate(reg_scene_);

	sys_camera.SetSpeed(1000);
	sys_light.SetGlobalLightPos({ 5000, 5000, -5000 });
	sys_light.OnCreate(reg_scene_);
	sys_effect.OnCreate(reg_scene_);
	sys_sound.OnCreate(reg_scene_);

	auto player_entity = SCENE_MGR->AddPlayer<Player>();
	sys_camera.TargetTag(reg_scene_, "Player");

	auto character_actor = SCENE_MGR->GetPlayer<Player>(0);
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

	INPUT_EVENT->SubscribeKeyEvent({ DIK_SPACE }, std::bind(&Player::Jump, character_actor), KEY_PUSH);

	std::function<void()> idle = std::bind(&Player::Idle, character_actor);
	INPUT_EVENT->SubscribeKeyEvent({ DIK_D }, idle, KEY_UP);
	INPUT_EVENT->SubscribeKeyEvent({ DIK_S }, idle, KEY_UP);
	INPUT_EVENT->SubscribeKeyEvent({ DIK_W }, idle, KEY_UP);
	INPUT_EVENT->SubscribeKeyEvent({ DIK_A }, idle, KEY_UP);

	INPUT_EVENT->SubscribeMouseEvent({ MouseButton::L_BUTTON }, std::bind(&Player::Fire, character_actor), KEY_HOLD);
	INPUT_EVENT->SubscribeMouseEvent({ MouseButton::L_BUTTON }, idle, KEY_UP);

	level.Create("DeadPoly_FullLevel_04.stmesh", "LevelVS.cso", "DeadPoly_Level_Collision_04.stmesh");
	//level.ImportGuideLines("../../Contents/BinaryPackage/DeadPoly_Blocking1.mapdat", GuideLine::GuideType::eBlocking);
	level.ImportGuideLines("../../Contents/BinaryPackage/DeadPoly_NpcTrack_01.mapdat", GuideLine::GuideType::eNpcTrack);

	QUADTREE->Init(&level, 3);
	QUADTREE->CreatePhysicsCS();
	
	environment_.CreateEnvironment();
	environment_.SetWorldTime(60, 60, true);
	environment_.SetSkyColorByTime(RGB_TO_FLOAT(201, 205, 204), RGB_TO_FLOAT(11, 11, 19));
	environment_.SetFogDistanceByTime(5000, 1000);
	environment_.SetLightProperty(0.2f, 0.2f);

	gw_property_.AddProperty<float>("FPS", &TIMER->fps);
	gw_property_.AddProperty<int>("calculating triagnles", &QUADTREE->calculating_triagnles);	

	EFFECT_MGR->SpawnEffect<FX_Flame>(XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f), XMQuaternionIdentity(), XMVectorSet(10.0f, 10.0f, 10.0f, 0.0f));
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

	}
	
	cur_zombie_created = SCENE_MGR->GetNumOfActor() - 1;

	sys_camera.OnUpdate(reg_scene_);
	sys_light.OnUpdate(reg_scene_);
	sys_movement.OnUpdate(reg_scene_);
	sys_effect.OnUpdate(reg_scene_);
	sys_sound.OnUpdate(reg_scene_);
	QUADTREE->Frame(&sys_camera);
	QUADTREE->RunPhysicsCS("CollisionDetectCS.cso");

	environment_.Update(&sys_camera, &sys_light);

	ingame_ui.OnUpdate();

	if (DINPUT->GetMouseState(L_BUTTON) == KeyState::KEY_PUSH)
		CreateImpactEffectFromRay();

	if (DINPUT->GetKeyState(DIK_G) == KeyState::KEY_PUSH)
		CreateExplosionEffectFromRay();

	CursorStateUpdate();
}

void InGameScene::OnRender()
{
	environment_.Render();
	
	level.Update();
	level.Render();

	sys_render.OnUpdate(reg_scene_);
	sys_ui.OnUpdate(reg_scene_);

	GUI->RenderWidgets();
}

void InGameScene::OnRelease()
{
	QUADTREE->Release();
	reality::RESOURCE->Release();
}

void InGameScene::CreateImpactEffectFromRay()
{
	// Make Muzzle when Shot
	auto player_transform = SCENE_MGR->GetPlayer<Player>(0)->GetTransformMatrix();
	XMVECTOR s, r, t;
	XMMatrixDecompose(&s, &r, &t, player_transform);
	EFFECT_MGR->SpawnEffect<FX_Muzzle>(t);

	RayShape ray = sys_camera.CreateFrontRay();

	RayCallback raycallback_node = QUADTREE->RaycastAdjustLevel(ray, 10000.0f);
	auto raycallback_pair = QUADTREE->RaycastAdjustActor(ray);
	if (raycallback_pair.first.success && raycallback_node.success)
	{
		if (raycallback_pair.first.distance < raycallback_node.distance)
		{
			EFFECT_MGR->SpawnEffectFromNormal<FX_BloodImpact>(raycallback_pair.first.point, raycallback_pair.first.normal);
		}
		else
			EFFECT_MGR->SpawnEffectFromNormal<FX_ConcreteImpact>(raycallback_node.point, raycallback_node.normal);
	}
	else if (raycallback_pair.first.success)
	{
		EFFECT_MGR->SpawnEffectFromNormal<FX_BloodImpact>(raycallback_pair.first.point, raycallback_pair.first.normal);
	}
	else if (raycallback_node.success)
		EFFECT_MGR->SpawnEffectFromNormal<FX_ConcreteImpact>(raycallback_node.point, raycallback_node.normal);
}

void InGameScene::CreateExplosionEffectFromRay()
{
	RayShape ray = sys_camera.CreateFrontRay();

	RayCallback raycallback_node = QUADTREE->RaycastAdjustLevel(ray, 10000.0f);
	auto raycallback_pair = QUADTREE->RaycastAdjustActor(ray);
	if (raycallback_pair.first.success && raycallback_node.success)
	{
		if (raycallback_pair.first.distance < raycallback_node.distance)
		{
			EFFECT_MGR->SpawnEffect<FX_Explosion>(raycallback_pair.first.point);
		}
		else
			EFFECT_MGR->SpawnEffect<FX_Explosion>(raycallback_node.point);
	}
	else if (raycallback_pair.first.success)
	{
		EFFECT_MGR->SpawnEffect<FX_Explosion>(raycallback_pair.first.point);
	}
	else if (raycallback_node.success)
		EFFECT_MGR->SpawnEffect<FX_Explosion>(raycallback_node.point);
}


void InGameScene::CursorStateUpdate()
{
	static bool b_show_cursor = false;
	if (DINPUT->GetKeyState(DIK_T) == KeyState::KEY_PUSH)
	{
		b_show_cursor = !b_show_cursor;
		ShowCursor(b_show_cursor);
	}

	if (!b_show_cursor)
		SetCursorPos(ENGINE->GetWindowSize().x / 2.0f, ENGINE->GetWindowSize().y / 2.0f);
}


