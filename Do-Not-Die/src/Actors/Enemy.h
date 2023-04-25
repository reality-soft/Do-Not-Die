#pragma once
#include "GameCharacter.h"

class Enemy : public GameCharacter
{
public:
	virtual void OnInit(entt::registry& registry) override;
	virtual void OnUpdate() override;
	void SetCharacterAnimation(string anim_id) const;

public:
	void SetMovement(const XMVECTOR& direction);
	void Jump();
	void Idle();
	void Attack();

public:
	float	 GetMaxHp() const override;
	float	 GetCurHp() const override;
	void SetCurHp(int hp) override;
	void TakeDamage(int damage) override;

public:
	virtual void SetBehaviorTree(const vector<XMVECTOR>& target_poses);
	void SetMeshId(const string& mesh_id);
	void ChasePlayer();

private:
	string mesh_id_;
	int cur_node = 0;

private:
	reality::BehaviorTree behavior_tree_;

public:
	bool is_hit_ = false;
	bool in_defense_bound_ = false;
	bool player_in_sight_ = true;
	bool is_attacking_ = true;
	bool is_attack_ended = false;

private:
	bool is_moving_ = false;

public:
	bool IsMoving() { return is_moving_; }
};

class EnemyMoveToTargets : public reality::ActionNode
{
public:
	EnemyMoveToTargets(entt::entity owner_id, XMVECTOR target_position)
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

		if (XMVector3Length(target_pos - cur_pos).m128_f32[0] < 10.0f) {
			return reality::BehaviorStatus::SUCCESS;
		}

		return reality::BehaviorStatus::RUNNING;
	}

protected:
	XMVECTOR target_position_;
	entt::entity owner_id_;
};

class EnemyFollowPlayer : public EnemyMoveToTargets
{
public:
	EnemyFollowPlayer(entt::entity enemy_id, XMVECTOR target_position)
		: EnemyMoveToTargets(enemy_id, target_position)
	{
	};

	virtual reality::BehaviorStatus Action() override
	{
		Enemy* owner = reality::SCENE_MGR->GetActor<Enemy>(owner_id_);
		if (owner->player_in_sight_) {
			return reality::BehaviorStatus::FAILURE;
		}
		return EnemyMoveToTargets::Action();
	}
};

class Attack : public reality::ActionNode
{
public:
	Attack(entt::entity owner_id, XMVECTOR target_position)
		: owner_id_(owner_id), target_position_(target_position)
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
	XMVECTOR target_position_;
	entt::entity owner_id_;
};

namespace reality {
	class ZombieAnimationStateMachine : public AnimationStateMachine {
	public:
		enum States {
			IDLE,
			MOVE,
			HIT,
			DIE
		};

		ZombieAnimationStateMachine(entt::entity owner_id, string skeletal_mesh_id, int range, string bone_name = "") : AnimationStateMachine(owner_id, skeletal_mesh_id, range, bone_name) {};

		virtual void OnInit() override {
			states_.insert({ IDLE, make_shared<Idle>() });
			states_.insert({ MOVE, make_shared<Move>() });
			states_.insert({ HIT, make_shared<Hit>() });
			states_.insert({ DIE, make_shared<Die>() });
			transitions_.insert({ IDLE, Transition(MOVE,[this](const AnimationStateMachine* animation_state_machine) {
					Enemy* enemy = SCENE_MGR->GetActor<Enemy>(owner_id_);
					if (enemy->IsMoving()) {
						return true;
					}
					else {
						return false;
					}
				})
				});
			transitions_.insert({ MOVE, Transition(IDLE,[this](const AnimationStateMachine* animation_state_machine) {
					Enemy* enemy = SCENE_MGR->GetActor<Enemy>(owner_id_);
					if (enemy->IsMoving() == false) {
						return true;
					}
					else {
						return false;
					}
				})
				});
			transitions_.insert({ MOVE, Transition(HIT,[this](const AnimationStateMachine* animation_state_machine) {
					Enemy* enemy = SCENE_MGR->GetActor<Enemy>(owner_id_);
					if (enemy->is_hit_ && enemy->GetCurHp() >= 0.) {
						return true;
					}
					else {
						return false;
					}
				})
				});
			transitions_.insert({ IDLE, Transition(HIT,[this](const AnimationStateMachine* animation_state_machine) {
					Enemy* enemy = SCENE_MGR->GetActor<Enemy>(owner_id_);
					if (enemy->is_hit_) {
						return true;
					}
					else {
						return false;
					}
				})
				});
			transitions_.insert({ HIT, Transition(IDLE,[this](const AnimationStateMachine* animation_state_machine) {
					Enemy* enemy = SCENE_MGR->GetActor<Enemy>(owner_id_);
					if (IsAnimationEnded() && enemy->GetCurHp() >= 0 && enemy->IsMoving() == false) {
						return true;
					}
					else {
						return false;
					}
				})
				});
			transitions_.insert({ HIT, Transition(MOVE,[this](const AnimationStateMachine* animation_state_machine) {
					Enemy* enemy = SCENE_MGR->GetActor<Enemy>(owner_id_);
					if (IsAnimationEnded() && enemy->GetCurHp() >= 0 && enemy->IsMoving() == true) {
						return true;
					}
					else {
						return false;
					}
				})
				});
			transitions_.insert({ MOVE, Transition(DIE,[this](const AnimationStateMachine* animation_state_machine) {
					Enemy* enemy = SCENE_MGR->GetActor<Enemy>(owner_id_);
					if (enemy->is_hit_ == true && enemy->GetCurHp() <= 0) {
						return true;
					}
					else {
						return false;
					}
				})
				});
			transitions_.insert({ IDLE, Transition(DIE,[this](const AnimationStateMachine* animation_state_machine) {
					Enemy* enemy = SCENE_MGR->GetActor<Enemy>(owner_id_);
					if (enemy->is_hit_ == true && enemy->GetCurHp() <= 0) {
						return true;
					}
					else {
						return false;
					}
				})
				});

			transitions_.insert({ DIE, Transition(IDLE,[this](const AnimationStateMachine* animation_state_machine) {
					Enemy* enemy = SCENE_MGR->GetActor<Enemy>(owner_id_);
					if (IsAnimationEnded() == true) {
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
			virtual void Enter(AnimationStateMachine* animation_state_machine) override
			{
				animation_state_machine->SetAnimation("Zombie_Idle_1_v2_IPC_Anim.anim", 0.3f);
			}
			virtual void Exit(AnimationStateMachine* animation_state_machine) override
			{
			}

			virtual void OnUpdate(AnimationStateMachine* animation_state_machine) override
			{
			}
		};

		class Move : public AnimationState {
		public:
			Move() : AnimationState(MOVE) {}
		public:
			virtual void Enter(AnimationStateMachine* animation_state_machine) override
			{
				animation_state_machine->SetAnimation("Zombie_Walk_F_6_Loop_IPC_Anim_Unreal Take.anim", 0.8f);
			}
			virtual void Exit(AnimationStateMachine* animation_state_machine) override
			{
			}
			virtual void OnUpdate(AnimationStateMachine* animation_state_machine) override
			{
			}
		};

		class Hit : public AnimationState {
		public:
			Hit() : AnimationState(HIT) {}
		public:
			virtual void Enter(AnimationStateMachine* animation_state_machine) override
			{
				animation_state_machine->SetAnimation("Zombie_Atk_KnockBack_1_IPC_Anim_Unreal Take.anim", 0.8f);
			}
			virtual void Exit(AnimationStateMachine* animation_state_machine) override
			{
				SCENE_MGR->GetActor<Enemy>(animation_state_machine->GetOwnerId())->is_hit_ = false;
			}
			virtual void OnUpdate(AnimationStateMachine* animation_state_machine) override
			{
				Enemy* enemy = SCENE_MGR->GetActor<Enemy>(animation_state_machine->GetOwnerId());
				enemy->CancelMovement();
			}
		};

		class Die : public AnimationState {
		public:
			Die() : AnimationState(DIE) {}
		public:
			virtual void Enter(AnimationStateMachine* animation_state_machine) override
			{
				animation_state_machine->SetAnimation("Zombie_Death_Back_Mid_1_IPC_Anim_Unreal Take.anim", 0.5f);
			}
			virtual void Exit(AnimationStateMachine* animation_state_machine) override
			{
				EVENT->PushEvent<DeleteActorEvent>(animation_state_machine->GetOwnerId());
			}
			virtual void OnUpdate(AnimationStateMachine* animation_state_machine) override
			{
				Enemy* enemy = SCENE_MGR->GetActor<Enemy>(animation_state_machine->GetOwnerId());
				enemy->CancelMovement();
			}
		};
	};
}