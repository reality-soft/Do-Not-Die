#include "AttackEvent.h"

void AttackEvent_SingleRay::Process()
{
	GameCharacter* character = SCENE_MGR->GetActor<GameCharacter>(actor_id_);
	if (character == nullptr)
		return;

	if (character->tag == "enemy")
		EnemyProcess();
	else if (character->tag == "player")
		PlayerProcess();
};

void AttackEvent_SingleRay::EnemyProcess()
{
	auto enemy_actor = SCENE_MGR->GetActor<BaseEnemy>(actor_id_);
	enemy_actor->is_attacking_ = true;

	auto callback_actor = QUADTREE->RaycastActorTargeted(ray, SCENE_MGR->GetPlayer<Player>(0)->entity_id_);
	if (callback_actor.success)
	{
		if (SCENE_MGR->GetActor<GameCharacter>(callback_actor.ent)->tag == "player")
		{
			hit_actor_ = callback_actor.ent;
			EVENT->PushEvent<TakeDamageEvent>(enemy_actor->GetStatus("default_damage")->GetCurrentValue(), hit_actor_);
			EFFECT_MGR->SpawnEffectFromNormal<FX_BloodImpact>(callback_actor.point, callback_actor.normal);
			auto ingame_scene = (InGameScene*)SCENE_MGR->GetScene(INGAME).get();
			ingame_scene->GetUIActor().Hitted();
		}
	}	
}

void AttackEvent_SingleRay::PlayerProcess()
{
	auto callback_total = QUADTREE->Raycast(ray, actor_id_);
	if (callback_total.success)
	{
		if (callback_total.is_actor)
		{
			hit_actor_ = callback_total.ent;

			NormalZombie* enemy_actor = SCENE_MGR->GetActor<NormalZombie>(hit_actor_);
			if (enemy_actor)
				enemy_actor->AddImpulse(GetRayDirection(ray), 300.0f);

			EVENT->PushEvent<TakeDamageEvent>(SCENE_MGR->GetPlayer<Player>(0)->GetStatus("gunfire_damage")->GetCurrentValue(), hit_actor_);
			EFFECT_MGR->SpawnEffectFromNormal<FX_BloodImpact>(callback_total.point, callback_total.normal);
		}
		else
		{
			EFFECT_MGR->SpawnEffectFromNormal<FX_ConcreteImpact>(callback_total.point, callback_total.normal);
		}
	}
}


void AttackEvent_BoundSphere::Process()
{
	GameCharacter* character_actor = SCENE_MGR->GetActor<GameCharacter>(actor_id_);
	if (character_actor == nullptr)
		return;
	
	if (character_actor->tag == "enemy" || character_actor->tag == "boss enemy")
		EnemyProcess();
	else if (character_actor->tag == "player")
		PlayerProcess();

	for (const auto& hit : hit_actors_)
	{
		EVENT->PushEvent<TakeDamageEvent>(damage_, hit);

		
		XMVECTOR hit_point = _XMVECTOR3(sphere_.center);
		XMVECTOR hit_normal = -character_actor->GetFront();
		EFFECT_MGR->SpawnEffectFromNormal<FX_MeleeImpact>(hit_point, hit_normal);

		auto general_zombie = SCENE_MGR->GetActor<NormalZombie>(hit);
		if (general_zombie)
		{
			general_zombie->AddImpulse(-hit_normal + XMVectorSet(0, 0.3, 0, 0), damage_ * 30);
		}
	}

	return;
}

void AttackEvent_BoundSphere::EnemyProcess()
{
	BaseEnemy* enemy_actor = SCENE_MGR->GetActor<BaseEnemy>(actor_id_);
	Player* player_actor = SCENE_MGR->GetPlayer<Player>(0);
	if (enemy_actor == nullptr || player_actor == nullptr)
		return;

	if (player_actor->GetCapsuleComponent()->hit_enable == false)
		return;

	if (CapsuleToSphere(player_actor->GetCapsuleComponent()->capsule, sphere_) == CollideType::INTERSECT)
	{
		hit_actors_.push_back(player_actor->entity_id_);
	}
}

void AttackEvent_BoundSphere::PlayerProcess()
{
	Player* player_actor = SCENE_MGR->GetPlayer<Player>(0);
	if (player_actor == nullptr)
		return;

	const auto& capsule_view = player_actor->reg_scene_->view<C_CapsuleCollision>();
	for (const auto& ent : capsule_view)
	{
		auto actor = SCENE_MGR->GetActor<Character>(ent);
		if (actor == nullptr)
			continue;

		if (actor->tag == "enemy" || actor->tag == "boss enemy")
		{
			const auto& capsule = player_actor->reg_scene_->get<C_CapsuleCollision>(ent).capsule;
			if (CapsuleToSphere(capsule, sphere_) == CollideType::INTERSECT)
			{
				hit_actors_.push_back(ent);
			}
		}
	}
}

void AttackEvent_AboutCar::Process()
{
	BaseEnemy* enemy = SCENE_MGR->GetActor<BaseEnemy>(actor_id_);
	if (enemy == nullptr)
		return;

	auto c_capsule = enemy->GetCapsuleComponent();
	if (c_capsule == nullptr)
		return;

	XMVECTOR point_b = GetTipBaseAB(c_capsule->capsule)[3];
	RayShape ray_about_car(point_b, point_b + enemy->GetFront() * 1000);
	auto callback_car = QUADTREE->RaycastCarOnly(ray_about_car);

	if (callback_car.success)
	{
		float damage = enemy->GetStatus("car_damage")->GetCurrentValue();
		*enemy->targeting_car_health = max(0, *enemy->targeting_car_health - damage);
	}
}