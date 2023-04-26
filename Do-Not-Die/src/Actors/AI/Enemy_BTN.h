#include "Engine_include.h"
#include "Enemy.h"

class EnemyMoveToTarget : public reality::ActionNode
{
public:
	EnemyMoveToTarget(entt::entity owner_id, XMVECTOR target_position)
		: owner_id_(owner_id), target_position_(target_position)
	{
	};

	virtual reality::BehaviorStatus Action() override
	{
		Enemy* enemy = reality::SCENE_MGR->GetActor<Enemy>(owner_id_);

		XMVECTOR cur_pos = enemy->GetCurPosition();
		XMVECTOR target_pos = target_position_;
		target_pos.m128_f32[1] = 0.0f;
		cur_pos.m128_f32[1] = 0.0f;
		cur_pos.m128_f32[3] = 0.0f;
		float distance = XMVector3Length(target_pos - cur_pos).m128_f32[0];
		XMVECTOR direction_vector = XMVector3Normalize(target_pos - cur_pos);
		enemy->SetMovement(direction_vector);

		if (XMVector3Length(target_pos - cur_pos).m128_f32[0] < 30.0f) {
			return reality::BehaviorStatus::SUCCESS;
		}

		return reality::BehaviorStatus::RUNNING;
	}

protected:
	XMVECTOR target_position_;
	entt::entity owner_id_;
};

class EnemyFollowPlayer : public EnemyMoveToTarget
{
public:
	EnemyFollowPlayer(entt::entity enemy_id, XMVECTOR target_position)
		: EnemyMoveToTarget(enemy_id, target_position)
	{
	};

	virtual reality::BehaviorStatus Action() override
	{
		Enemy* owner = reality::SCENE_MGR->GetActor<Enemy>(owner_id_);
		XMVECTOR prev_target_position = target_position_;
		Player* player = reality::SCENE_MGR->GetPlayer<Player>(0);
		target_position_ = player->GetCurPosition();

		if (owner->player_in_sight_ == false || player->player_in_defense_ == false) {
			return reality::BehaviorStatus::FAILURE;
		}
		return EnemyMoveToTarget::Action();
	}
};

class EnemyFollowCar : public EnemyMoveToTarget
{
public:
	EnemyFollowCar(entt::entity enemy_id, XMVECTOR target_position)
		: EnemyMoveToTarget(enemy_id, target_position)
	{
	};

	virtual reality::BehaviorStatus Action() override
	{
		Enemy* owner = reality::SCENE_MGR->GetActor<Enemy>(owner_id_);
		XMVECTOR owner_pos = owner->GetCurPosition();

		Player* player = reality::SCENE_MGR->GetPlayer<Player>(0);
		if (owner->player_in_sight_ && player->player_in_defense_ == true) {
			return reality::BehaviorStatus::FAILURE;
		}

		if (status_ == reality::BehaviorStatus::IDLE) {
			map<UINT, XMVECTOR> car_attack_poses = reality::QUADTREE->GetGuideLines("DND_CarAttack_1")->at(0).line_nodes;

			float min_distance = FLT_MAX;

			for (const auto& car_attack_pos_pair : car_attack_poses) {
				const XMVECTOR& car_attack_pos = car_attack_pos_pair.second;

				float cur_distance = XMVector3Length(car_attack_pos - owner_pos).m128_f32[0];
				if (cur_distance <= min_distance) {
					min_distance = cur_distance;
					target_position_ = car_attack_pos;
				}
			}
		}

		return EnemyMoveToTarget::Action();
	}
};

class EnemyAttack : public reality::ActionNode
{
public:
	EnemyAttack(entt::entity owner_id)
		: owner_id_(owner_id)
	{
	};

	virtual reality::BehaviorStatus Action() override
	{
		Enemy* enemy = reality::SCENE_MGR->GetActor<Enemy>(owner_id_);

		if (enemy->is_attacking_ == true) {
			return reality::BehaviorStatus::RUNNING;
		}
		if (enemy->is_attacking_ == false && enemy->is_attack_ended == true) {
			enemy->is_attack_ended = false;
			return reality::BehaviorStatus::SUCCESS;
		}
		if (enemy->is_attacking_ == false && enemy->is_attack_ended == false) {
			enemy->Attack();
			return reality::BehaviorStatus::RUNNING;
		}
	}

protected:
	entt::entity owner_id_;
};