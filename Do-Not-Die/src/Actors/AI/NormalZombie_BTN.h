#pragma once
#include "Engine_include.h"
#include "NormalZombie.h"
#include "Player.h"

class NormalZombieFollowPlayer : public reality::ActionNode
{
public:
	NormalZombieFollowPlayer(entt::entity enemy_id, XMVECTOR target_position)
		: owner_id_(enemy_id), target_position_(target_position)
	{
	};

	virtual reality::BehaviorStatus Action() override
	{
		static float search_time = 0.0f;
		search_time += TM_DELTATIME;

		Player* player = reality::SCENE_MGR->GetPlayer<Player>(0);
		BaseEnemy* enemy = reality::SCENE_MGR->GetActor<BaseEnemy>(owner_id_);

		XMVECTOR direction_to_player = XMVector3Normalize(player->GetCurPosition() - enemy->GetCurPosition());
		target_position_ = player->GetCurPosition();

		if (enemy->player_in_sight_ == false || player->player_in_defense_ == false)
			return reality::BehaviorStatus::FAILURE;


		float distance_to_player = Distance(player->GetCurPosition(), enemy->GetCurPosition());

		if (distance_to_player >= 30)
		{
			enemy->SetMovement(direction_to_player);
		}
		if (Distance(player->GetCurPosition(), enemy->GetCurPosition()) < 50.0f)
		{
			enemy->Attack();
		}

		return reality::BehaviorStatus::RUNNING;
	}

protected:
	XMVECTOR target_position_;
	entt::entity owner_id_;
};