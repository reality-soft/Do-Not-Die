#include "StartScene.h"
#include "FbxMgr.h"
#include "FX_Flame.h"

void StartScene::OnInit()
{
	loading_progress = START_RESOURCE;
	reality::RESOURCE->Init("../../Contents/");

	loading_progress = START_MANAGER;
	COMPONENT->OnInit(reg_scene_);

	loading_progress = START_SYSTEM;
	sys_render.OnCreate(reg_scene_);
	sys_ui.OnCreate(reg_scene_);
	sys_sound.OnCreate(reg_scene_);
	sys_effect.OnCreate(reg_scene_);
	sys_light.OnCreate(reg_scene_);
	sys_light.SetGlobalLightPos({ 5000, 5000, -5000 });

	reality::ComponentSystem::GetInst()->OnInit(reg_scene_);

	loading_progress = START_ACTOR;

	intro_scene_ui.OnInit(reg_scene_);
	intro_scene_ui.SetIntroTime(6.0f);

	sequence_camera_ = Scene::AddActor<SequenceCameraActor>();
	sequence_camera_actor = Scene::GetActor<SequenceCameraActor>(sequence_camera_);
	sequence_camera_actor->ImportSequenceTrack("DND_SequenceTrack_2.mapdat");
	start_scene_ui.OnInit(reg_scene_);

	level.Create("DNDLevel_WithCollision_01.stmesh", "LevelVS.cso");

	environment_.CreateEnvironment();
	environment_.SetWorldTime(120, 120);
	environment_.SetSkyColorByTime(RGB_TO_FLOAT(201, 205, 204), RGB_TO_FLOAT(11, 11, 19));
	environment_.SetFogDistanceByTime(5000, 1000);
	environment_.SetLightProperty(0.2f, 0.2f);
	fire_effects_.Import("DND_StartSceneFire_1.mapdat", GuideType::eSpawnPoint);

	for (const auto& node : fire_effects_.line_nodes)
	{
		int random_scale = RandomFloatInRange(10.0f, 15.0f);
		entt::entity ent = EFFECT_MGR->SpawnEffect<reality::FX_Flame>(START, node.second, XMQuaternionIdentity(), XMVectorReplicate(random_scale));
		Scene::GetActor<Actor>(ent)->visible = true;
	}

	loading_progress = START_FINISHED;
}

void StartScene::OnUpdate()
{
	intro_scene_ui.OnUpdate();
	start_scene_ui.OnUpdate();

	if (intro_scene_ui.intro_end_)
	{
		sequence_camera_actor->playable = true;
	}

	sys_effect.OnUpdate(reg_scene_);
	sys_light.OnUpdate(reg_scene_);
	sys_sound.OnUpdate(reg_scene_);
	environment_.Update(sequence_camera_actor->world_pos_, &sys_light);
}

void StartScene::OnRender()
{
	environment_.Render();

	level.Update();
	level.Render();

	sys_render.OnUpdate(reg_scene_);
	sys_ui.OnUpdate(reg_scene_);
}

void StartScene::OnRelease()
{

}
