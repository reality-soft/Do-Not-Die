#pragma once
#include "Engine_include.h"
#include "AnimationStateMachine.h"
#include "AnimationState.h"
#include "BossZombie.h"
using namespace reality;

class BossZombieBaseAnimationStateMachine : public AnimationStateMachine {
public:
	enum States {
		IDLE,
		MOVE,
		PUNCH_ATTACK,
		JUMP_ATTACK
	};

	BossZombieBaseAnimationStateMachine(entt::entity owner_id, string skeletal_mesh_id, int range, string bone_name = "") : AnimationStateMachine(owner_id, skeletal_mesh_id, range, bone_name) {};

	virtual void OnInit() override {
		states_.insert({ IDLE, make_shared<Idle>() });
		states_.insert({ MOVE, make_shared<Move>() });
		states_.insert({ PUNCH_ATTACK, make_shared<PunchAttack>() });
		states_.insert({ JUMP_ATTACK, make_shared<JumpAttack>() });
		transitions_.insert({ IDLE, Transition(MOVE,[this](const AnimationStateMachine* animation_state_machine) {
				BaseEnemy* enemy = SCENE_MGR->GetActor<BaseEnemy>(owner_id_);
				if (enemy->is_moving_) {
					return true;
				}
				else {
					return false;
				}
			})
			});
		transitions_.insert({ MOVE, Transition(IDLE,[this](const AnimationStateMachine* animation_state_machine) {
				BaseEnemy* enemy = SCENE_MGR->GetActor<BaseEnemy>(owner_id_);
				if (enemy->is_moving_ == false) {
					return true;
				}
				else {
					return false;
				}
			})
			});

		// Punch Attack
		{
			transitions_.insert({ IDLE, Transition(PUNCH_ATTACK,[this](const AnimationStateMachine* animation_state_machine) {
				BossZombie* enemy = SCENE_MGR->GetActor<BossZombie>(owner_id_);
				if (enemy->is_attacking_ == true && enemy->cur_attack_type_ == BossZombieAttackType::PUNCH_ATTACK) {
					return true;
				}
				else {
					return false;
				}
			})
			});
			transitions_.insert({ MOVE, Transition(PUNCH_ATTACK,[this](const AnimationStateMachine* animation_state_machine) {
				BossZombie* enemy = SCENE_MGR->GetActor<BossZombie>(owner_id_);
				if (enemy->is_attacking_ == true && enemy->cur_attack_type_ == BossZombieAttackType::PUNCH_ATTACK) {
					return true;
				}
				else {
					return false;
				}
			})
			});

			transitions_.insert({ PUNCH_ATTACK, Transition(IDLE,[this](const AnimationStateMachine* animation_state_machine) {
				if (IsAnimationEnded()) {
					return true;
				}
				else {
					return false;
				}
			})
			});
		}

		// Jump Attack
		{
			transitions_.insert({ IDLE, Transition(JUMP_ATTACK,[this](const AnimationStateMachine* animation_state_machine) {
				BossZombie* enemy = SCENE_MGR->GetActor<BossZombie>(owner_id_);
				if (enemy->is_attacking_ == true && enemy->cur_attack_type_ == BossZombieAttackType::JUMP_ATTACK) {
					return true;
				}
				else {
					return false;
				}
			})
			});
			transitions_.insert({ MOVE, Transition(JUMP_ATTACK,[this](const AnimationStateMachine* animation_state_machine) {
				BossZombie* enemy = SCENE_MGR->GetActor<BossZombie>(owner_id_);
				if (enemy->is_attacking_ == true && enemy->cur_attack_type_ == BossZombieAttackType::JUMP_ATTACK) {
					return true;
				}
				else {
					return false;
				}
			})
			});

			transitions_.insert({ JUMP_ATTACK, Transition(IDLE,[this](const AnimationStateMachine* animation_state_machine) {
				if (IsAnimationEnded()) {
					return true;
				}
				else {
					return false;
				}
			})
			});
		}


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
			animation_state_machine->SetAnimation("DND_BossZombie_Idle.anim", 0.3f, true);
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
			vector<AnimNotify> notifies;
			notifies.push_back({ 1, make_shared<SoundGenerateEvent>(animation_state_machine->GetOwnerId(), SFX, "Zombie_Move_1.mp3", 0.5f, false), true });
			animation_state_machine->SetAnimation("DND_BossZombie_Walk.anim", 0.8f, true, notifies);
		}
		virtual void Exit(AnimationStateMachine* animation_state_machine) override
		{
		}
		virtual void OnUpdate(AnimationStateMachine* animation_state_machine) override
		{
		}
	};

	class PunchAttack : public AnimationState {
	public:
		PunchAttack() : AnimationState(PUNCH_ATTACK) {}
	public:
		virtual void Enter(AnimationStateMachine* animation_state_machine) override
		{
			vector<AnimNotify> notifies;
			animation_state_machine->SetAnimation("DND_BossZombie_StrongPunch.anim", 0.8f, true, notifies);
		}
		virtual void Exit(AnimationStateMachine* animation_state_machine) override
		{
			BossZombie* enemy = SCENE_MGR->GetActor<BossZombie>(animation_state_machine->GetOwnerId());
			enemy->is_attack_ended_ = true;
			enemy->is_attacking_ = false;
		}
		virtual void OnUpdate(AnimationStateMachine* animation_state_machine) override
		{
			BossZombie* enemy = SCENE_MGR->GetActor<BossZombie>(animation_state_machine->GetOwnerId());
			enemy->CancelMovement();
		}
	};

	class JumpAttack : public AnimationState {
	public:
		JumpAttack() : AnimationState(JUMP_ATTACK) {}
	public:
		virtual void Enter(AnimationStateMachine* animation_state_machine) override
		{
			vector<AnimNotify> notifies;
			animation_state_machine->SetAnimation("DND_BossZombie_JumpAttack.anim", 0.8f, true, notifies);
		}
		virtual void Exit(AnimationStateMachine* animation_state_machine) override
		{
			BossZombie* enemy = SCENE_MGR->GetActor<BossZombie>(animation_state_machine->GetOwnerId());
			enemy->is_attack_ended_ = true;
			enemy->is_attacking_ = false;
		}
		virtual void OnUpdate(AnimationStateMachine* animation_state_machine) override
		{
			BossZombie* enemy = SCENE_MGR->GetActor<BossZombie>(animation_state_machine->GetOwnerId());
			enemy->CancelMovement();
		}
	};
};