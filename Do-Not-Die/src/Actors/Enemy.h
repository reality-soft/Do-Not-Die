#pragma once
#include "GameCharacter.h"

class Enemy : public GameCharacter
{
public:
	virtual void OnInit(entt::registry& registry) override;
	virtual void OnUpdate() override;
	void SetCharacterAnimation(string anim_id) const;

public:
	void Move();
	void Jump();
	void Idle();
	void Attack();

public:
	float	 GetMaxHp() const override;
	float	 GetCurHp() const override;
	void SetCurHp(int hp) override;
	void TakeDamage(int damage) override;

public:
	void SetDirection(const XMVECTOR& direction);
	void SetRoute(const vector<XMVECTOR>& target_poses);
	void SetMeshId(const string& mesh_id);

private:
	string mesh_id_;
	int cur_node = 0;

private:
	reality::BehaviorTree behavior_tree_;

private:
	bool is_moving_ = false;

public:
	bool IsMoving() { return is_moving_; }
};

class EnemyMoveToTargets : public reality::ActionNode
{
public:
	EnemyMoveToTargets(entt::entity enemy_id, XMVECTOR target_position)
		: enemy_id_(enemy_id), target_position_(target_position)
	{
	};

	virtual reality::BehaviorStatus Action() override
	{
		Enemy* enemy = reality::SCENE_MGR->GetActor<Enemy>(enemy_id_);

		enemy->Move();

		XMVECTOR cur_pos = enemy->GetPos();
		enemy->SetDirection(XMVector3Normalize(target_position_ - cur_pos));
		enemy->RotateAlongMovementDirection();

		if (XMVector3Length(target_position_ - cur_pos).m128_f32[0] < 100.0f) {
			return reality::BehaviorStatus::SUCCESS;
		}

		return reality::BehaviorStatus::RUNNING;
	}

private:
	XMVECTOR target_position_;
	entt::entity enemy_id_;
};

namespace reality {
	class ZombieAnimationStateMachine : public AnimationStateMachine {
	public:
		enum States {
			IDLE,
			MOVE,
		};

		ZombieAnimationStateMachine(entt::entity owner_id) : AnimationStateMachine(owner_id) {};

		virtual void OnInit() override {
			states_.insert({ IDLE, make_shared<Idle>() });
			states_.insert({ MOVE, make_shared<Move>() });
			transitions_.insert({ IDLE, Transition(MOVE,[this](const AnimationBase* animation_base) {
					Enemy* enemy = SCENE_MGR->GetActor<Enemy>(owner_id_);
					if (enemy->IsMoving()) {
						return true;
					}
					else {
						return false;
					}
				})
				});
			transitions_.insert({ MOVE, Transition(IDLE,[this](const AnimationBase* animation_base) {
					Enemy* enemy = SCENE_MGR->GetActor<Enemy>(owner_id_);
					if (enemy->IsMoving() == false) {
						return true;
					}
					else {
						return false;
					}
				})
				});

			cur_state_ = states_[IDLE];
		}

		virtual void OnUpdate() override {
			AnimationStateMachine::OnUpdate();
		}

		class Idle : public AnimationState {
		public:
			Idle() : AnimationState(IDLE) {}
		public:
			virtual void Enter(AnimationStateMachine* animation_base) override
			{
				animation_base->SetAnimation("Zombie_Idle_1_v2_IPC_Anim.anim", 0.3f);
			}
			virtual void Exit(AnimationStateMachine* animation_base) override
			{
			}

			virtual void OnUpdate(AnimationStateMachine* animation_base) override
			{
			}
		};

		class Move : public AnimationState {
		public:
			Move() : AnimationState(MOVE) {}
		public:
			virtual void Enter(AnimationStateMachine* animation_base) override
			{
				animation_base->SetAnimation("Zombie_Walk_F_6_Loop_IPC_Anim_Unreal Take.anim", 0.3f);
			}
			virtual void Exit(AnimationStateMachine* animation_base) override
			{
			}
			virtual void OnUpdate(AnimationStateMachine* animation_base) override
			{
			}
		};
	};
}