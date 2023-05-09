#pragma once
#include "Engine_include.h"
#include "AnimationStateMachine.h"
#include "AnimationState.h"
#include "BaseEnemy.h"
using namespace reality;

class BossZombieBaseAnimationStateMachine : public AnimationStateMachine {
public:
	enum States {
		IDLE_BASE,
		MOVE_BASE,
	};

	BossZombieBaseAnimationStateMachine(entt::entity owner_id, string skeletal_mesh_id, int range, string bone_name = "") : AnimationStateMachine(owner_id, skeletal_mesh_id, range, bone_name) {};

	virtual void OnInit() override {
		states_.insert({ IDLE_BASE, make_shared<Idle_Base>() });
		states_.insert({ MOVE_BASE, make_shared<Move_Base>() });
		transitions_.insert({ IDLE_BASE, Transition(MOVE_BASE,[this](const AnimationStateMachine* animation_state_machine) {
				BaseEnemy* enemy = SCENE_MGR->GetActor<BaseEnemy>(owner_id_);
				if (enemy->is_moving_) {
					return true;
				}
				else {
					return false;
				}
			})
			});
		transitions_.insert({ MOVE_BASE, Transition(IDLE_BASE,[this](const AnimationStateMachine* animation_state_machine) {
				BaseEnemy* enemy = SCENE_MGR->GetActor<BaseEnemy>(owner_id_);
				if (enemy->is_moving_ == false) {
					return true;
				}
				else {
					return false;
				}
			})
			});

		cur_state_ = states_[IDLE_BASE];
	}

	virtual void OnUpdate() override {
		AnimationStateMachine::OnUpdate();
	}

	class Idle_Base : public AnimationState {
	public:
		Idle_Base() : AnimationState(IDLE_BASE) {}
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

	class Move_Base : public AnimationState {
	public:
		Move_Base() : AnimationState(MOVE_BASE) {}


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
};

class BossZombieUpperBodyAnimationStateMachine : public AnimationStateMachine {
public:
	enum States {
		IDLE,
		MOVE,
		ATTACK
	};

	BossZombieUpperBodyAnimationStateMachine(entt::entity owner_id, string skeletal_mesh_id, int range, string bone_name = "") : AnimationStateMachine(owner_id, skeletal_mesh_id, range, bone_name) {};

	virtual void OnInit() override {
		states_.insert({ IDLE, make_shared<Idle>() });
		states_.insert({ MOVE, make_shared<Move>() });
		states_.insert({ ATTACK, make_shared<Attack>() });
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
		transitions_.insert({ IDLE, Transition(ATTACK,[this](const AnimationStateMachine* animation_state_machine) {
				BaseEnemy* enemy = SCENE_MGR->GetActor<BaseEnemy>(owner_id_);
				if (enemy->is_attacking_ == true) {
					return true;
				}
				else {
					return false;
				}
			})
			});

		transitions_.insert({ MOVE, Transition(ATTACK,[this](const AnimationStateMachine* animation_state_machine) {
				BaseEnemy* enemy = SCENE_MGR->GetActor<BaseEnemy>(owner_id_);
				if (enemy->is_attacking_ == true) {
					return true;
				}
				else {
					return false;
				}
			})
			});

		transitions_.insert({ ATTACK, Transition(IDLE,[this](const AnimationStateMachine* animation_state_machine) {
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
			animation_state_machine->SetAnimation("DND_BossZombie_Walk.anim", 0.8f, true);
		}
		virtual void Exit(AnimationStateMachine* animation_state_machine) override
		{
		}
		virtual void OnUpdate(AnimationStateMachine* animation_state_machine) override
		{
		}
	};

	class Attack : public AnimationState {
	public:
		Attack() : AnimationState(ATTACK) {}
	public:
		virtual void Enter(AnimationStateMachine* animation_state_machine) override
		{
			EVENT->PushEvent<SoundGenerateEvent>(animation_state_machine->GetOwnerId(), SFX, "ZombieAttack_1.mp3", 0.5f, false);
			animation_state_machine->SetAnimation("DND_BossZombie_StrongPunch.anim", 0.5f, true);
		}
		virtual void Exit(AnimationStateMachine* animation_state_machine) override
		{
			BaseEnemy* enemy = SCENE_MGR->GetActor<BaseEnemy>(animation_state_machine->GetOwnerId());
			enemy->is_attack_ended_ = true;
			enemy->is_attacking_ = false;
		}
		virtual void OnUpdate(AnimationStateMachine* animation_state_machine) override
		{
			BaseEnemy* enemy = SCENE_MGR->GetActor<BaseEnemy>(animation_state_machine->GetOwnerId());
			//enemy->CancelMovement();
		}
	};
};