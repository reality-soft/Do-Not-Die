#pragma once
#include "Engine_include.h"
#include "AnimationStateMachine.h"
#include "AnimationState.h"
#include "BossZombie.h"
#include "FX_Range.h"
#include "FX_Smash.h"
#include "BossZombie.h"
using namespace reality;

class BossZombieBaseAnimationStateMachine : public AnimationStateMachine {
public:
	enum States {
		IDLE,
		MOVE,
		LEFT_RIGHT_HOOK,
		KICK_ATTACK,
		PUNCH_ATTACK,
		JUMP_ATTACK
	};

	BossZombieBaseAnimationStateMachine(entt::entity owner_id, string skeletal_mesh_id, int range, string bone_name = "") : AnimationStateMachine(owner_id, skeletal_mesh_id, range, bone_name) {};

	virtual void OnInit() override {
		states_.insert({ IDLE, make_shared<Idle>() });
		states_.insert({ MOVE, make_shared<Move>() });
		states_.insert({ LEFT_RIGHT_HOOK, make_shared<LeftRightHook>() });
		states_.insert({ KICK_ATTACK, make_shared<KickAttack>() });
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

		// Left Right Hook
		{
			transitions_.insert({ IDLE, Transition(LEFT_RIGHT_HOOK,[this](const AnimationStateMachine* animation_state_machine) {
				BossZombie* enemy = SCENE_MGR->GetActor<BossZombie>(owner_id_);
				if (enemy->is_attacking_ == true && enemy->cur_attack_type_ == BossZombieAttackType::LEFT_RIGHT_HOOK) {
					return true;
				}
				else {
					return false;
				}
			})
			});
			transitions_.insert({ MOVE, Transition(LEFT_RIGHT_HOOK,[this](const AnimationStateMachine* animation_state_machine) {
				BossZombie* enemy = SCENE_MGR->GetActor<BossZombie>(owner_id_);
				if (enemy->is_attacking_ == true && enemy->cur_attack_type_ == BossZombieAttackType::LEFT_RIGHT_HOOK) {
					return true;
				}
				else {
					return false;
				}
			})
			});

			transitions_.insert({ LEFT_RIGHT_HOOK, Transition(IDLE,[this](const AnimationStateMachine* animation_state_machine) {
				if (GetCurAnimationId() == "DND_BossZombie_Punch_R.anim" && IsAnimationEnded()) {
					return true;
				}
				else {
					return false;
				}
			})
			});
		}

		// Kick Attack
		{
			transitions_.insert({ IDLE, Transition(KICK_ATTACK,[this](const AnimationStateMachine* animation_state_machine) {
				BossZombie* enemy = SCENE_MGR->GetActor<BossZombie>(owner_id_);
				if (enemy->is_attacking_ == true && enemy->cur_attack_type_ == BossZombieAttackType::KICK_ATTACK) {
					return true;
				}
				else {
					return false;
				}
			})
			});
			transitions_.insert({ MOVE, Transition(KICK_ATTACK,[this](const AnimationStateMachine* animation_state_machine) {
				BossZombie* enemy = SCENE_MGR->GetActor<BossZombie>(owner_id_);
				if (enemy->is_attacking_ == true && enemy->cur_attack_type_ == BossZombieAttackType::KICK_ATTACK) {
					return true;
				}
				else {
					return false;
				}
			})
			});

			transitions_.insert({ KICK_ATTACK, Transition(IDLE,[this](const AnimationStateMachine* animation_state_machine) {
				if (IsAnimationEnded()) {
					return true;
				}
				else {
					return false;
				}
			})
			});
		}

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
			animation_state_machine->SetAnimation("DND_BossZombie_Run.anim", 0.8f, true, notifies);
		}
		virtual void Exit(AnimationStateMachine* animation_state_machine) override
		{
		}
		virtual void OnUpdate(AnimationStateMachine* animation_state_machine) override
		{
		}
	};

	class LeftRightHook : public AnimationState {
	public:
		LeftRightHook() : AnimationState(LEFT_RIGHT_HOOK) {}
	public:
		virtual void Enter(AnimationStateMachine* animation_state_machine) override
		{
			left_hook = true;
			right_hook = true;

			owner_actor_ = SCENE_MGR->GetActor<BossZombie>(animation_state_machine->GetOwnerId());
			owner_actor_->rotate_enable_ = false;

			animation_state_machine->SetAnimation("DND_BossZombie_Punch_L.anim", 0.8f, true, notifies);
		}
		virtual void Exit(AnimationStateMachine* animation_state_machine) override
		{
			owner_actor_->rotate_enable_ = true;
			owner_actor_->is_attacking_ = false;
			owner_actor_->is_attack_ended_ = true;
			owner_actor_ = nullptr;
		}
		virtual void OnUpdate(AnimationStateMachine* animation_state_machine) override
		{
			owner_actor_->CancelMovement();
			if (animation_state_machine->GetCurAnimation().cur_frame_ >= 28 && animation_state_machine->GetCurAnimationId() == "DND_BossZombie_Punch_L.anim")
				animation_state_machine->SetAnimation("DND_BossZombie_Punch_R.anim", 0.8f, true, notifies);

			if ((int)animation_state_machine->GetCurAnimation().cur_frame_ == 15)
			{
				if (left_hook && animation_state_machine->GetCurAnimationId() == "DND_BossZombie_Punch_L.anim")
				{
					owner_actor_->LeftHook(); 
					left_hook = false;
				}
			}
			if ((int)animation_state_machine->GetCurAnimation().cur_frame_ == 20)
			{
				if (right_hook && animation_state_machine->GetCurAnimationId() == "DND_BossZombie_Punch_R.anim")
				{
					owner_actor_->RightHook();
					right_hook = false;
				}
			}
		}

		int left_hook = true;
		int right_hook = true;
		BossZombie* owner_actor_ = nullptr;
		vector<AnimNotify> notifies;
	};

	class KickAttack : public AnimationState {
	public:
		KickAttack() : AnimationState(KICK_ATTACK) {}
	public:
		virtual void Enter(AnimationStateMachine* animation_state_machine) override
		{
			kick_ = true;

			owner_actor_ = SCENE_MGR->GetActor<BossZombie>(animation_state_machine->GetOwnerId());
			owner_actor_->rotate_enable_ = false;

			animation_state_machine->SetAnimation("DND_BossZombie_Kick.anim", 0.8f, true, notifies);
		}
		virtual void Exit(AnimationStateMachine* animation_state_machine) override
		{
			owner_actor_->rotate_enable_ = true;
			owner_actor_->is_attacking_ = false;
			owner_actor_->is_attack_ended_ = true;
			owner_actor_ = nullptr;
		}
		virtual void OnUpdate(AnimationStateMachine* animation_state_machine) override
		{
			owner_actor_->CancelMovement();

			if (kick_ && (int)animation_state_machine->GetCurAnimation().cur_frame_ == 40)
			{
				owner_actor_->KickAttack();
				kick_ = false;
			}			
		}

		bool kick_ = true;
		BossZombie* owner_actor_ = nullptr;
		vector<AnimNotify> notifies;
	};

	class PunchAttack : public AnimationState {
	public:
		PunchAttack() : AnimationState(PUNCH_ATTACK) {}
	public:
		virtual void Enter(AnimationStateMachine* animation_state_machine) override
		{
			punch_ = true;

			owner_actor_ = SCENE_MGR->GetActor<BossZombie>(animation_state_machine->GetOwnerId());
			owner_actor_->rotate_enable_ = false;

			vector<AnimNotify> notifies;
			animation_state_machine->SetAnimation("DND_BossZombie_StrongPunch.anim", 0.8f, true, notifies);
		}
		virtual void Exit(AnimationStateMachine* animation_state_machine) override
		{
			owner_actor_->rotate_enable_ = true;
			owner_actor_->is_attacking_ = false;
			owner_actor_->is_attack_ended_ = true;
			owner_actor_ = nullptr;
		}
		virtual void OnUpdate(AnimationStateMachine* animation_state_machine) override
		{
			owner_actor_->CancelMovement();

			if (punch_ && (int)animation_state_machine->GetCurAnimation().cur_frame_ == 80)
			{
				owner_actor_->PunchAttack();
				punch_ = false;
			}
		}

		bool punch_ = true;
		BossZombie* owner_actor_ = nullptr;
	};

	class JumpAttack : public AnimationState {
	public:
		JumpAttack() : AnimationState(JUMP_ATTACK) {}
	public:
		virtual void Enter(AnimationStateMachine* animation_state_machine) override
		{
			//jump_ = true;

			owner_actor_ = SCENE_MGR->GetActor<BossZombie>(animation_state_machine->GetOwnerId());
			owner_actor_->rotate_enable_ = false;

			vector<AnimNotify> notifies;
			animation_state_machine->SetAnimation("DND_BossZombie_JumpAttack.anim", 0.8f, true, notifies);

		}
		virtual void Exit(AnimationStateMachine* animation_state_machine) override
		{
			owner_actor_->rotate_enable_ = true;
			owner_actor_->is_attacking_ = false;
			owner_actor_->is_attack_ended_ = true;
			owner_actor_ = nullptr;
		}
		virtual void OnUpdate(AnimationStateMachine* animation_state_machine) override
		{
			owner_actor_->CancelMovement();
			int frame = animation_state_machine->GetCurAnimation().cur_frame_;
			if (!jump_ && frame == 36)
			{
				EFFECT_MGR->SpawnEffect<FX_Range>(owner_actor_->GetCurPosition(), XMQuaternionIdentity(), { 200.0f, 200.0f, 200.0f, 1.0f });
				jump_ = true;
			}
			if (jump_ && (int)animation_state_machine->GetCurAnimation().cur_frame_ == 94)
			{
				owner_actor_->JumpAttack();
				EFFECT_MGR->SpawnEffect<FX_Smash>(owner_actor_->GetCurPosition(), XMQuaternionIdentity(), { 200.0f, 200.0f, 200.0f, 1.0f });
				jump_ = false;
			}
		}

		bool jump_ = true;
		BossZombie* owner_actor_ = nullptr;
	};
};