#include "EndingScene.h"

void EndingScene::OnInit()
{
	loading_progress = ENDING_RESOURCE;

	loading_progress = ENDING_MANAGER;

	loading_progress = ENDING_SYSTEM;
	reality::ComponentSystem::GetInst()->OnInit(reg_scene_);

	sys_render.OnCreate(reg_scene_);
	sys_ui.OnCreate(reg_scene_);
	sys_sound.OnCreate(reg_scene_);
	sys_effect.OnCreate(reg_scene_);
	sys_light.OnCreate(reg_scene_);
	sys_light.SetGlobalLightPos({ 5000, 5000, -5000 , 0 });
	sys_animation.OnCreate(reg_scene_);

	loading_progress = ENDING_ACTOR;

	riding_car_actor = GetActor<RidingCar>(AddActor<RidingCar>());

 	sequence_camera_actor = GetActor<SequenceCameraActor>(AddActor<SequenceCameraActor>());
	sequence_camera_actor->ImportSequenceTrack("DND_EndingSequence_2.mapdat");
	sequence_camera_actor->playable = true;
	sequence_camera_actor->acc = 1.0f;
	sequence_camera_actor->play_speed = 500.0f;
	sequence_camera_actor->keep_sequence_at_last_direction = true;

	level.Create("DNDLevel_WithCollision_01.stmesh", "LevelVS.cso");

	environment_.CreateEnvironment();
	environment_.SetWorldTime(30, 0);
	environment_.SetSkyColorByTime(RGB_TO_FLOAT(201, 205, 204), RGB_TO_FLOAT(11, 11, 19));
	environment_.SetFogDistanceByTime(10000, 5000);
	environment_.SetLightProperty(XMFLOAT4(1.0, 0.7, 0.5, 1), XMFLOAT4(0.05, 0.05, 0.1, 1), 0.1f, 0.25f);

	ending_ui_.OnInit(reg_scene_);

	loading_progress = ENDING_FINISHED;
}

void EndingScene::OnUpdate()
{
	if (DINPUT->GetKeyState(DIK_TAB))
		scene_finished = true;

	scene_finished = ending_ui_.ending_finished;
	if (scene_finished)
		FinishProgress();
	else
	{
		sys_sound.PlayBackground("MichaelFK_FleetingBeauty_cut.wav", false, 0.0f, 1.0f);
	}

	sys_animation.OnUpdate(reg_scene_);
	sys_effect.OnUpdate(reg_scene_);
	sys_light.OnUpdate(reg_scene_);
	sys_sound.OnUpdate(reg_scene_);
	environment_.Update(sequence_camera_actor->world_pos_, &sys_light);

	ending_ui_.OnUpdate();
}

void EndingScene::OnRender()
{
	environment_.Render();

	level.Update();
	level.Render();

	sys_render.OnUpdate(reg_scene_);
	sys_ui.OnUpdate(reg_scene_);
}

void EndingScene::OnRelease()
{
	reality::RESOURCE->Release();
}

void EndingScene::StartingProgress()
{

}
void EndingScene::FinishProgress()
{
	bool sound_finished = sys_sound.FadeOutDelete("MichaelFK_FleetingBeauty_cut.wav", 5.0f);
	bool fade_out_finished = ending_ui_.FadeInBg();

	if (sound_finished && fade_out_finished)
		DestroyWindow(ENGINE->GetWindowHandle());
}