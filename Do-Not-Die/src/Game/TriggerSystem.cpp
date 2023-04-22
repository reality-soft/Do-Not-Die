#include "TriggerSystem.h"
#include "TriggerEvent.h"

using namespace reality;

void reality::TriggerSystem::OnCreate(entt::registry& reg)
{

}

void reality::TriggerSystem::OnUpdate(entt::registry& reg)
{
	auto trigger_view = reg.view<C_TriggerVolume>();
	if (trigger_view.empty())
		return;

	const auto& scene_actors = SCENE_MGR->GetScene(E_SceneType::INGAME)->GetActors();

	CheckCurrentTriggerValid(reg);

	for (const auto& target_actor : scene_actors)
	{
		if (target_actor.second.get()->trigger_sensor == false)
			continue;

		for (const auto& trigger_entity : trigger_view)
		{
			const auto& c_trigger = reg.get<C_TriggerVolume>(trigger_entity);
			if (IsActorInTrigger(target_actor.first, c_trigger))
			{
				if (IsAlreadyTrigged(target_actor.first, trigger_entity))
					continue;

				current_triggers.insert(make_pair(target_actor.first, trigger_entity));
				EVENT->PushEvent<TriggerEvent>(target_actor.first, trigger_entity, true);
			}
			else
			{
				if (IsAlreadyTrigged(target_actor.first, trigger_entity))
				{
					EVENT->PushEvent<TriggerEvent>(target_actor.first, trigger_entity, false);
					current_triggers.erase(make_pair(target_actor.first, trigger_entity));
				}
			}
		}
	}
}

void reality::TriggerSystem::AddTriggerAtActor(entt::entity ent, float radius)
{
	auto actor = SCENE_MGR->GetActor<Actor>(ent);
	if (actor == nullptr)
		return;

	C_TriggerVolume new_trigger;
	new_trigger.sphere_volume.center = _XMFLOAT3(actor->GetCurPosition());
	new_trigger.sphere_volume.radius = radius;

	SCENE_MGR->GetScene(E_SceneType::INGAME)->GetRegistryRef().emplace_or_replace<C_TriggerVolume>(ent, new_trigger);
}

bool reality::TriggerSystem::IsActorInTrigger(entt::entity ent, const C_TriggerVolume& trigger)
{
	XMVECTOR actor_position = SCENE_MGR->GetActor<Actor>(ent)->GetCurPosition();
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
