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

	auto scene_actors = SCENE_MGR->GetScene(E_SceneType::INGAME)->GetActors();

	for (auto target_actor : scene_actors)
	{
		if (target_actor.second.get()->trigger_sensor == false)
			continue;

		for (auto trigger_entity : trigger_view)
		{
			const auto& c_trigger = reg.get<C_TriggerVolume>(trigger_entity);
			if (IsActorInTrigger(target_actor.first, c_trigger))
			{
				if (current_triggers.find(target_actor.first) != current_triggers.end())
				{
					if (current_triggers.at(target_actor.first) == trigger_entity) // already trigged
					{
						continue;
					}
				}

				current_triggers.insert(make_pair(target_actor.first, trigger_entity));
				EVENT->PushEvent<TriggerEvent>(target_actor.first, trigger_entity, true);
			}
			else
			{
				if (current_triggers.find(target_actor.first) != current_triggers.end())
				{
					if (current_triggers.at(target_actor.first) == trigger_entity) // already trigged
					{
						EVENT->PushEvent<TriggerEvent>(target_actor.first, trigger_entity, false);
						current_triggers.erase(target_actor.first);
					}
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
