#include "Engine_include.h"
#include "BaseEnemy.h"
// common
class EnemyMoveToTarget : public reality::ActionNode
{
public:
	EnemyMoveToTarget(entt::entity owner_id, XMVECTOR target_position)
		: owner_id_(owner_id), target_position_(target_position)
	{
	};

	virtual reality::BehaviorStatus Action() override
	{
		BaseEnemy* enemy = reality::SCENE_MGR->GetActor<BaseEnemy>(owner_id_);

		XMVECTOR cur_pos = enemy->GetCurPosition();
		XMVECTOR target_pos = target_position_;
		target_pos.m128_f32[1] = 0.0f;
		cur_pos.m128_f32[1] = 0.0f;
		cur_pos.m128_f32[3] = 0.0f;
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

class EnemyFollowCar : public reality::ActionNode
{
public:
	EnemyFollowCar(entt::entity enemy_id, XMVECTOR target_position)
		: owner_id_(enemy_id), target_position_(target_position)
	{
	};

	virtual reality::BehaviorStatus Action() override
	{
  		BaseEnemy* owner = reality::SCENE_MGR->GetActor<BaseEnemy>(owner_id_);
		XMVECTOR owner_pos = owner->GetCurPosition();

		Player* player = reality::SCENE_MGR->GetPlayer<Player>(0);
		if (owner->player_in_sight_ && player->player_in_defense_ == true && player->is_dead_ == false && player->is_zombie_ == false) {
			return reality::BehaviorStatus::FAILURE;
		}

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

		XMVECTOR dir = XMVector3Normalize(target_position_ - owner_pos);
		owner->SetMovement(dir);

		float distance = 0;
		C_CapsuleCollision* c_capsule = owner->reg_scene_->try_get<C_CapsuleCollision>(owner_id_);
		if (c_capsule)
		{
			auto capsule_info = GetTipBaseAB(c_capsule->capsule);
			distance = XMVector3LinePointDistance(capsule_info[0], capsule_info[1], target_position_).m128_f32[0];
		}
		
		if (distance <= 10)
		{
			owner->Attack();
		}

		return reality::BehaviorStatus::RUNNING;
	}

protected:
	XMVECTOR target_position_;
	entt::entity owner_id_;
};