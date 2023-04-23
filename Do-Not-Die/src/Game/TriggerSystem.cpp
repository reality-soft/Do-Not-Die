#include "TriggerSystem.h"
#include "TriggerEvent.h"

using namespace reality;

void reality::TriggerSystem::OnCreate(entt::registry& reg)
{
	const auto& trigger_view = reg.view<C_TriggerVolume>();
	if (trigger_view.empty())
		return;

	for (const auto& ent_trigger : trigger_view)
	{
		const auto& trigger_volume = reg.get<C_TriggerVolume>(ent_trigger);
		if (trigger_volume.tag == "defense")
			defense_trigger_ = ent_trigger; break;
	}
	
}

void reality::TriggerSystem::OnUpdate(entt::registry& reg)
{
	const auto& sensor_view = reg.view<C_TriggerSensor>();
	if (sensor_view.empty())
		return;

	const auto& trigger_view = reg.view<C_TriggerVolume>();
	if (trigger_view.empty())
		return;

	CheckCurrentTriggerValid(reg);

	//UINT sensor_count = sensor_view.size();
	//UINT trigger_count = trigger_view.size();

	//for (const auto& ent_sensor : sensor_view)
	//{
	//	const auto& trigger_sensor = reg.get<C_TriggerSensor>(ent_sensor);

	//	for (const auto& ent_trigger : trigger_view)
	//	{
	//		const auto& trigger_volume = reg.get<C_TriggerVolume>(ent_trigger);

	//		if (trigger_sensor.can_sense_tags.find(trigger_volume.tag) != trigger_sensor.can_sense_tags.end())
	//		{
	//			if (IsActorInTrigger(ent_sensor, trigger_volume))
	//			{
	//				if (IsAlreadyTrigged(ent_sensor, ent_trigger))
	//					continue;

	//				current_triggers.insert(make_pair(ent_sensor, ent_trigger));
	//				EVENT->PushEvent<TriggerEvent>(ent_sensor, ent_trigger, true);
	//			}
	//			else
	//			{
	//				if (IsAlreadyTrigged(ent_sensor, ent_trigger))
	//				{
	//					EVENT->PushEvent<TriggerEvent>(ent_sensor, ent_trigger, false);
	//					current_triggers.erase(make_pair(ent_sensor, ent_trigger));
	//				}
	//			}
	//		}
	//	}
	//}

	// player
	auto player = SCENE_MGR->GetPlayer<Player>(0);
	if (player == nullptr)
		return;

	for (const auto& ent_trigger : trigger_view)
	{
		const auto& trigger_volume = reg.get<C_TriggerVolume>(ent_trigger);

		if (IsActorInTrigger(player->entity_id_, trigger_volume))
		{
			if (IsAlreadyTrigged(player->entity_id_, ent_trigger))
				continue;

			current_triggers.insert(make_pair(player->entity_id_, ent_trigger));
			EVENT->PushEvent<TriggerEvent>(player->entity_id_, ent_trigger, true);
		}
		else
		{
			if (IsAlreadyTrigged(player->entity_id_, ent_trigger))
			{
				EVENT->PushEvent<TriggerEvent>(player->entity_id_, ent_trigger, false);
				current_triggers.erase(make_pair(player->entity_id_, ent_trigger));
			}
		}		
	}

	// enemy
	for (const auto& ent_sensor : sensor_view)
	{
		const auto& trigger_volume = reg.get<C_TriggerVolume>(defense_trigger_);
		if (ent_sensor == player->entity_id_)
			continue;

		if (IsActorInTrigger(ent_sensor, trigger_volume))
		{
			if (IsAlreadyTrigged(ent_sensor, defense_trigger_))
				continue;

			current_triggers.insert(make_pair(ent_sensor, defense_trigger_));
			EVENT->PushEvent<TriggerEvent>(ent_sensor, defense_trigger_, true);
		}
		else
		{
			if (IsAlreadyTrigged(ent_sensor, defense_trigger_))
			{
				EVENT->PushEvent<TriggerEvent>(ent_sensor, defense_trigger_, false);
				current_triggers.erase(make_pair(ent_sensor, defense_trigger_));
			}
		}
	}



	//const auto& scene_actors = SCENE_MGR->GetScene(E_SceneType::INGAME)->GetActors();

	//CheckCurrentTriggerValid(reg);

	//for (const auto& target_actor : scene_actors)
	//{
	//	for (const auto& trigger_entity : trigger_view)
	//	{
	//		const auto& c_trigger = reg.get<C_TriggerVolume>(trigger_entity);
	//		if (target_actor.second.get()->tag == "enemy" && c_trigger.tag == "item")
	//			continue;

	//		if (target_actor.second.get()->tag == "enemy" && c_trigger.tag == "extract")
	//			continue;

	//		if (IsActorInTrigger(target_actor.first, c_trigger))
	//		{
	//			if (IsAlreadyTrigged(target_actor.first, trigger_entity))
	//				continue;

	//			current_triggers.insert(make_pair(target_actor.first, trigger_entity));
	//			EVENT->PushEvent<TriggerEvent>(target_actor.first, trigger_entity, true);
	//		}
	//		else
	//		{
	//			if (IsAlreadyTrigged(target_actor.first, trigger_entity))
	//			{
	//				EVENT->PushEvent<TriggerEvent>(target_actor.first, trigger_entity, false);
	//				current_triggers.erase(make_pair(target_actor.first, trigger_entity));
	//			}
	//		}
	//	}
	//}
}

void reality::TriggerSystem::AddTriggerAtActor(entt::entity ent, float radius)
{
	auto actor = SCENE_MGR->GetActor<Actor>(ent);
	if (actor == nullptr)
		return;

	C_TriggerVolume new_trigger;
	new_trigger.sphere_volume.center = _XMFLOAT3(actor->GetTransformMatrix().r[3]);
	new_trigger.sphere_volume.radius = radius;

	SCENE_MGR->GetScene(E_SceneType::INGAME)->GetRegistryRef().emplace_or_replace<C_TriggerVolume>(ent, new_trigger);
}

bool reality::TriggerSystem::IsActorInTrigger(entt::entity ent, const C_TriggerVolume& trigger)
{
	XMVECTOR actor_position = SCENE_MGR->GetActor<Actor>(ent)->GetTransformMatrix().r[3];
	if (Distance(actor_position, _XMVECTOR3(trigger.sphere_volume.center)) <= trigger.sphere_volume.radius)
		return true;

	return false;
}

void reality::TriggerSystem::CheckCurrentTriggerValid(entt::registry& reg)
{
	vector<pair<entt::entity, entt::entity>> to_erase;

	for (auto& pair : current_triggers)
	{
		if (reg.valid(pair.second))
			continue;
		else
		{
			EVENT->PushEvent<TriggerEvent>(pair.first, pair.second, false);
			to_erase.push_back(pair);
		}
	}

	for (auto pair : to_erase)
	{
		current_triggers.erase(pair);
	}
}

bool reality::TriggerSystem::IsAlreadyTrigged(entt::entity target_actor, entt::entity trigger_actor)
{
	for (const auto& pair : current_triggers)
	{
		if (pair.first == target_actor && pair.second == trigger_actor)
			return true;
	}

	return false;	
}
