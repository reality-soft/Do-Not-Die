#pragma once
#include "Engine_include.h"
#include "BossZombie.h"

class BossZombieFollowPlayer : public reality::ActionNode
{
public:
	BossZombieFollowPlayer(entt::entity enemy_id)
		: owner_id_(enemy_id)
	{
	};

	virtual reality:: BehaviorStatus Action() override
	{
		Player* player = reality::SCENE_MGR->GetPlayer<Player>(0);
		BaseEnemy* enemy = reality::SCENE_MGR->GetActor<BaseEnemy>(owner_id_);

		if (enemy->player_in_sight_ == false || player->player_in_defense_ == false)
			return reality::BehaviorStatus::FAILURE;

		XMVECTOR direction_to_player = XMVector3Normalize(player->GetCurPosition() - enemy->GetCurPosition());
		enemy->SetMovement(direction_to_player);

		return reality::BehaviorStatus::RUNNING;
	}

protected:
	entt::entity owner_id_;
};

class BossPunchAttack : public reality::ActionNode
{
public:
	BossPunchAttack(entt::entity enemy_id)
		: owner_id_(enemy_id)
	{
	};

	virtual reality::BehaviorStatus Action() override
	{
		static float search_time = 0.0f;
		search_time += TM_DELTATIME;

		Player* player = reality::SCENE_MGR->GetPlayer<Player>(0);
		BossZombie* enemy = reality::SCENE_MGR->GetActor<BossZombie>(owner_id_);

		if (enemy->is_attacking_ == false) {
			enemy->PunchAttack();
		}
		if (enemy->is_attack_ended_ == true) {
			enemy->is_attacking_ = false;
			enemy->is_attack_ended_ = false;
			return BehaviorStatus::SUCCESS;
		}

		return BehaviorStatus::RUNNING;
	}

protected:
	entt::entity owner_id_;
};

class BossJumpAttack : public reality::ActionNode
{
public:
	BossJumpAttack(entt::entity enemy_id)
		: owner_id_(enemy_id)
	{
	};

	virtual reality::BehaviorStatus Action() override
	{
		static float search_time = 0.0f;
		search_time += TM_DELTATIME;

		Player* player = reality::SCENE_MGR->GetPlayer<Player>(0);
		BossZombie* enemy = reality::SCENE_MGR->GetActor<BossZombie>(owner_id_);

		if (enemy->is_attacking_ == false) {
			enemy->JumpAttack();
		}
		if (enemy->is_attack_ended_ == true) {
			enemy->is_attacking_ = false;
			enemy->is_attack_ended_ = false;
			return BehaviorStatus::SUCCESS;
		}

		return BehaviorStatus::RUNNING;
	}

protected:
	entt::entity owner_id_;
};