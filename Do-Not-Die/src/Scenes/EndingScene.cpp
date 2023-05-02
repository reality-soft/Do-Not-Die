#include "EndingScene.h"

void EndingScene::OnInit()
{
	loading_progress = ENDING_RESOURCE;

	loading_progress = ENDING_MANAGER;

	loading_progress = ENDING_SYSTEM;
	sys_render.OnCreate(reg_scene_);
	sys_ui.OnCreate(reg_scene_);
	sys_sound.OnCreate(reg_scene_);
	sys_effect.OnCreate(reg_scene_);
	sys_light.OnCreate(reg_scene_);
	sys_light.SetGlobalLightPos({ -5000, 5000, -5000 });
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
	environment_.SetWorldTime(60, 0);
	environment_.SetSkyColorByTime(RGB_TO_FLOAT(201, 205, 204), RGB_TO_FLOAT(11, 11, 19));
	environment_.SetFogDistanceByTime(10000, 5000);
	environment_.SetLightProperty(0.2f, 0.2f);

	ending_ui_.OnInit(reg_scene_);

	loading_progress = ENDING_FINISHED;
}

void EndingScene::OnUpdate()
{
	if (DINPUT->GetKeyState(DIK_TAB))
		scene_finished = true;

	if (scene_finished)
		FinishProgress();

	else
	{
		sys_sound.PlayBackground("MichaelFK_Empyrean_cut.wav", true, 10.0f, 1.0f);
		//sys_sound.PlayBackground("TonyAnderson_Nuit_cut.wav", true, 10.0f, 1.0f);
		//sys_sound.PlayBackground("NathanWhitehead_DaysGone_cut.wav", true, 10.0f, 1.0f);
		//sys_sound.PlayBackground("ZackHemsey_TheWay_cut.wav", true, 10.0f, 1.0f);
		//sys_sound.PlayBackground("Saw_HelloZepp_cut.wav", true, 10.0f, 1.0f);
		//sys_sound.PlayBackground("FarCry5_OhJohn_cut.wav", true, 10.0f, 1.0f);

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
}

void EndingScene::StartingProgress()
{

}
void EndingScene::FinishProgress()
{

}