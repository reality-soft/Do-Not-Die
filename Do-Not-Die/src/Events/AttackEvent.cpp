#include "AttackEvent.h"

void AttackEvent_SingleRay::Process()
{
	GameCharacter* character = SCENE_MGR->GetActor<GameCharacter>(actor_id_);
	if (character == nullptr)
		return;

	entt::entity actor_hit;
	float damage = character->GetCharacterDamage();

	if (character->tag == "enemy")
	{
		auto enemy_actor = SCENE_MGR->GetActor<GeneralZombie>(actor_id_);
		enemy_actor->is_attacking_ = true;

		auto callback_car = QUADTREE->RaycastCarOnly(ray);
		if (callback_car.success)
		{
			*enemy_actor->targeting_car_health = max(0, *enemy_actor->targeting_car_health - 5);
		}
		else
		{
			auto callback_actor = QUADTREE->RaycastActorTargeted(ray, SCENE_MGR->GetPlayer<Player>(0)->entity_id_);
			if (callback_actor.success)
			{
				if (SCENE_MGR->GetActor<GameCharacter>(callback_actor.ent)->tag == "player")
				{
					actor_hit = callback_actor.ent;
					EVENT->PushEvent<TakeDamageEvent>(damage, actor_hit);
					EFFECT_MGR->SpawnEffectFromNormal<FX_BloodImpact>(callback_actor.point, callback_actor.normal);
				}
			}
		}

	}
	else if (character->tag == "player")
	{
		auto callback_total = QUADTREE->Raycast(ray, actor_id_);
		if (callback_total.success)
		{
			if (callback_total.is_actor)
			{
				actor_hit = callback_total.ent;

				GeneralZombie* enemy_actor = SCENE_MGR->GetActor<GeneralZombie>(actor_hit);
				if (enemy_actor)
					enemy_actor->AddImpulse(GetRayDirection(ray), 1000.0f);

				EVENT->PushEvent<TakeDamageEvent>(damage, actor_hit);
				EFFECT_MGR->SpawnEffectFromNormal<FX_BloodImpact>(callback_total.point, callback_total.normal);
			}
			else
			{
				EFFECT_MGR->SpawnEffectFromNormal<FX_ConcreteImpact>(callback_total.point, callback_total.normal);
			}
		}
	}
};

void AttackEvent_BoundSphere::Process()
{
	GameCharacter* character_actor = SCENE_MGR->GetActor<GameCharacter>(actor_id_);
	if (character_actor == nullptr)
		return;

	if (character_actor->tag == "enemy")
		EnemyProcess();
	else if (character_actor->tag == "player")
		PlayerProcess();

	return;
}

void AttackEvent_BoundSphere::EnemyProcess()
{
	GeneralZombie* enemy_actor = SCENE_MGR->GetActor<GeneralZombie>(actor_id_);
	Player* player_actor = SCENE_MGR->GetPlayer<Player>(0);
	if (enemy_actor == nullptr || player_actor == nullptr)
		return;

	if (CapsuleToSphere(player_actor->GetCapsuleComponent()->capsule, sphere_) == CollideType::INTERSECT)
	{		
		EVENT->PushEvent<TakeDamageEvent>(enemy_actor->GetCharacterDamage(), player_actor->entity_id_);
	}
}

void AttackEvent_BoundSphere::PlayerProcess()
{

}