#include "WaveSystem.h"
#include "GameEvents.h"
#include "Player.h"
#include "NormalZombie.h"
#include "BossZombie.h"
#include "Item.h"
#include "FX_Flame.h"
#include "FX_Smoke.h"

using namespace reality;

void reality::WaveSystem::OnCreate(entt::registry& reg)
{
	item_spawns_ = QUADTREE->GetGuideLines("DND_ItemSpawn_1")->at(0);
	repair_spawns_ = QUADTREE->GetGuideLines("DND_RepairPart_1")->at(0);
	car_event_ = QUADTREE->GetGuideLines("DND_CarEvent_1")->at(0);
	zomebie_tracks_ = QUADTREE->GetGuideLines("DND_NpcTrack_1");
	boss_track_ = QUADTREE->GetGuideLines("DND_BossTrack_1")->at(0);
	fx_car_fire_ = QUADTREE->GetGuideLines("DND_FX_CarFire_1")->at(0);
	fx_corpse_fire_ = QUADTREE->GetGuideLines("DND_FX_CorpseFire_1")->at(0);

	CreateExtractPoints(reg);
	CreateCarEventTriggers(_XMFLOAT3(car_event_.line_nodes.begin()->second), 150, 800);
	CreateStaticEffects();

	wave_ui_.OnInit(reg);
}

void reality::WaveSystem::OnUpdate(entt::registry& reg)
{
	float counting_time = world_env_->GetCountingTime(); 
	if (world_env_->IsDayChanged())
	{
		switch (world_env_->GetCurrentDay())
		{
		case Day::eNoon:
			WaveFinish();
			countdown_timer_ = fabs(world_env_->GetTimeLimits().x * 2);
			break;
		case Day::eNight:
			WaveStart();
			countdown_timer_ = fabs(world_env_->GetTimeLimits().y * 2);
			break;
		}
	}
	zombie_count = SCENE_MGR->GetNumOfActor("enemy");
	countdown_timer_ -= TM_DELTATIME;
	PlayerExtractRepair();
	PlayerRepairCar();
	SpawnZombies(3.f);
	SpawnCarSmokes();

	if (car_health <= 0)
	{
		EVENT->PushEvent<GameResultEvent>(GameResultType::eCarCrashed);
	}

	wave_ui_.OnUpdate();
}

void reality::WaveSystem::SetWorldEnv(Environment* env)
{
	world_env_ = shared_ptr<Environment>(env);
	countdown_timer_ = fabs(world_env_->GetTimeLimits().x * 2);
}

void reality::WaveSystem::RandomSpawnItem(float trigger_radius)
{
	list<UINT> empty_item_index;

	const auto& item_view = SCENE_MGR->GetScene(INGAME)->GetRegistryRef().view<C_TriggerVolume>();
	for (const auto& spawn : item_spawns_.line_nodes)
	{
		bool is_item_stay = false;

		for (const auto& ent : item_view)
		{
			const auto& trigger = SCENE_MGR->GetScene(INGAME)->GetRegistryRef().get<C_TriggerVolume>(ent);
			if (trigger.tag != "item")
				continue;

			if (Distance(spawn.second, _XMVECTOR3(trigger.sphere_volume.center)) < trigger_radius)
				is_item_stay = true;
		}

		if (!is_item_stay)
		{
			ItemType item_type = (ItemType)RandomIntInRange(0, 7);
			SCENE_MGR->GetScene(INGAME)->AddActor<Item>(item_type, _XMFLOAT3(spawn.second), trigger_radius);
		}
	}
}

void reality::WaveSystem::CreateCarEventTriggers(XMFLOAT3 point, float repair_radius, float defense_radius)
{
	entt::entity reapair_volume_ent = SCENE_MGR->GetScene(INGAME).get()->GetRegistryRef().create();
	entt::entity defense_volume_ent = SCENE_MGR->GetScene(INGAME).get()->GetRegistryRef().create();

	C_TriggerVolume repair_trigger;
	repair_trigger.tag = "repair";
	repair_trigger.sphere_volume.center = point;
	repair_trigger.sphere_volume.radius = repair_radius;
	SCENE_MGR->GetScene(INGAME).get()->GetRegistryRef().emplace<C_TriggerVolume>(reapair_volume_ent, repair_trigger);
	
	C_TriggerVolume defense_trigger;
	defense_trigger.tag = "defense";
	defense_trigger.sphere_volume.center = point;
	defense_trigger.sphere_volume.radius = defense_radius;
	SCENE_MGR->GetScene(INGAME).get()->GetRegistryRef().emplace<C_TriggerVolume>(defense_volume_ent, defense_trigger);
}

void reality::WaveSystem::CreateStaticEffects()
{
	for (const auto& node : fx_corpse_fire_.line_nodes)
	{
		float random_scale = RandomFloatInRange(5.0f, 10.0f);
		entt::entity ent = EFFECT_MGR->SpawnEffect<reality::FX_Flame>(INGAME, node.second, XMQuaternionIdentity(), XMVectorReplicate(random_scale));
	}
}

void reality::WaveSystem::CreateExtractPoints(entt::registry& reg)
{
	for (const auto& node : repair_spawns_.line_nodes)
	{
		C_TriggerVolume trigger;
		trigger.tag = "extract";

		trigger.sphere_volume.center = _XMFLOAT3(node.second);
		trigger.sphere_volume.radius = 100;

		entt::entity trigger_entity = reg.create();
		reg.emplace<C_TriggerVolume>(trigger_entity, trigger);
	}
}

void reality::WaveSystem::PlayerExtractRepair()
{
	auto player = SCENE_MGR->GetPlayer<Player>(0);
	if (player == nullptr)
		return;

	if (player->can_extract_repair_ == false)
		return;

	auto trigger_comp = SCENE_MGR->GetScene(INGAME)->GetRegistryRef().try_get<C_TriggerVolume>(player->repair_extract_trigger);
	if (trigger_comp == nullptr)
		return;

	if (DINPUT->GetKeyState(DIK_E) == KEY_HOLD)
	{
		player->extract_during_time_ += TM_DELTATIME;
		player->InteractionRotate(_XMVECTOR3(trigger_comp->sphere_volume.center));
	}

	if (DINPUT->GetKeyState(DIK_E) == KEY_UP)
	{
		player->extract_during_time_ = 0.0f;
		player->controller_enable_ = true;
	}

	if (player->extract_during_time_ >= player->extract_time_takes_)
	{
		auto trigger_comp = SCENE_MGR->GetScene(INGAME)->GetRegistryRef().try_get<C_TriggerVolume>(player->repair_extract_trigger);
		if (trigger_comp == nullptr)
			return;

		SpawnRepairItem(trigger_comp->sphere_volume.center);

		EVENT->PushEvent<MakeTextEvent>("Extract Success!", 1920.0f / 2.0f - 100.0f);

		DeleteExtractPoint(player->repair_extract_trigger);
		player->extract_during_time_ = 0.0f;
		player->can_extract_repair_ = false;
		player->controller_enable_ = true;
	}
}

void reality::WaveSystem::PlayerRepairCar()
{
	auto player = SCENE_MGR->GetPlayer<Player>(0);
	if (player == nullptr)
		return;

	if (player->can_repair_car_ == false)
		return;

	if(player->HasRepairPart() == false)
	{
		if(DINPUT->GetKeyState(DIK_E) == KEY_PUSH)
			EVENT->PushEvent<MakeTextEvent>("There is no Repair Part in Inventory!", 1920.0f / 2.0f - 250.0f);
		return;
	}
		

	if (DINPUT->GetKeyState(DIK_E) == KEY_UP)
	{
		player->controller_enable_ = true;
		player->repair_during_time_ = 0.0f;
	}

	if (DINPUT->GetKeyState(DIK_E) == KEY_HOLD)
	{
		player->repair_during_time_ += TM_DELTATIME;
		player->InteractionRotate(car_event_.line_nodes.begin()->second);		
	}

	if (player->repair_during_time_ >= player->repair_time_takes_)
	{
		player->controller_enable_ = true;
		player->UseRepairPart();
		car_health = min(car_health_max, car_health + 20);
		player->repair_during_time_ = 0.0f; 
		car_repair_count++;
		EVENT->PushEvent<MakeTextEvent>("Repair Success!", 1920.0f / 2.0f - 100.0f);
	}
}

void reality::WaveSystem::SpawnRepairItem(XMFLOAT3 position)
{
	SCENE_MGR->AddActor<Item>(ItemType::eRepairPart, position, 30.0f);
}

void reality::WaveSystem::DeleteExtractPoint(entt::entity ent)
{
	auto player = SCENE_MGR->GetPlayer<Player>(0);
	if (player == nullptr)
		return;

	if (SCENE_MGR->GetScene(INGAME)->GetRegistryRef().valid(ent))
	{
		SCENE_MGR->GetScene(INGAME)->GetRegistryRef().destroy(ent);
	}
}

void reality::WaveSystem::SpawnZombies(float interval)
{
	static float cur_time = 0.0f;
	cur_time += TM_DELTATIME;

	if (zombie_spawn_count_ <= 0)
	{
		cur_time = 0.0f;
		return;
	}

	if (cur_time < interval)
		return;

	auto enemy_actor = SCENE_MGR->GetActor<NormalZombie>(SCENE_MGR->AddActor<NormalZombie>());
	if (enemy_actor == nullptr)
		return;

	enemy_actor->targeting_car_health = &car_health;
	
	int guidline_index = RandomIntInRange(0, zomebie_tracks_->size() - 1);
	int mesh_index = RandomIntInRange(0, enemy_meshes.size() - 1);

	vector<XMVECTOR> target_poses;
	for (const auto& target_pos : zomebie_tracks_->at(guidline_index).line_nodes) {
		target_poses.push_back(target_pos.second);
	}
	enemy_actor->SetBehaviorTree(target_poses);
	enemy_actor->SetMeshId(enemy_meshes[mesh_index]);

	cur_time = 0.0f;
	zombie_spawn_count_ -= 1;	
}

void reality::WaveSystem::SpawnBossZombie()
{
	BossZombie* boss = SCENE_MGR->GetScene(INGAME)->GetActor<BossZombie>(SCENE_MGR->GetScene(INGAME)->AddActor<BossZombie>());
	boss_zombie_ent = boss->entity_id_;

	boss->targeting_car_health = &car_health;

	vector<XMVECTOR> target_poses;
	for (const auto& target_pos : boss_track_.line_nodes) {
		target_poses.push_back(target_pos.second);
	}
	boss->SetBehaviorTree(target_poses);

	boss_zombie_spawn = true;
}

void reality::WaveSystem::SpawnCarSmokes()
{
	for (int i = 0; i < car_health_max; i += car_health_max / 5)
	{
		int index = i / 20;

		if (i >= car_health || car_health == 0)
		{
			if (spawned_car_fired_.find(index) != spawned_car_fired_.end())
				continue;

			auto node = fx_car_fire_.line_nodes.at(index);
			float random_scale = RandomFloatInRange(5.0f, 10.0f);
			entt::entity ent = EFFECT_MGR->SpawnEffect<reality::FX_Smoke>(INGAME, node, XMQuaternionIdentity(), XMVectorReplicate(random_scale));
			spawned_car_fired_.insert(make_pair(index, ent));
		}
		else
		{
			if (spawned_car_fired_.find(index) != spawned_car_fired_.end())
			{
				SCENE_MGR->DestroyActor(spawned_car_fired_.find(index)->second);
				spawned_car_fired_.erase(index);
			}
		}
	}
}

XMVECTOR reality::WaveSystem::GetCarPosition()
{
	return car_event_.line_nodes.begin()->second;
}


void reality::WaveSystem::WaveStart()
{
	wave_ui_.ShowWaveStart();

	if (wave_count_ == 4)
		SpawnBossZombie();
	else
		zombie_spawn_count_ += 20;

	FMOD_MGR->Stop("S_Day_BGM.wav");
	FMOD_MGR->Play("S_Night_BGM.wav", SoundType::MUSIC, true, 1.0f, {});
	FMOD_MGR->Play("WaveStart.wav", SoundType::MUSIC, false, 0.5f, {});
}

void reality::WaveSystem::WaveFinish()
{
	wave_ui_.ShowWaveFinish();

	FMOD_MGR->Stop("S_Night_BGM.wav");
	FMOD_MGR->Play("S_Day_BGM.wav", SoundType::MUSIC, true, 1.0f, {});
	FMOD_MGR->Play("WaveFinish.wav", SoundType::MUSIC, false, 0.5f, {});
	wave_count_++;
	RandomSpawnItem(30);
}
