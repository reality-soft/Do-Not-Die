#pragma once
#include "Engine_include.h"
#include "AnimationStateMachine.h"
#include "AnimationState.h"
#include "Enemy.h"
using namespace reality;

class ZombieAnimationStateMachine : public AnimationStateMachine {
public:
	enum States {
		IDLE,
		MOVE,
		HIT,
		DIE,
		ATTACK
	};

	ZombieAnimationStateMachine(entt::entity owner_id, string skeletal_mesh_id, int range, string bone_name = "") : AnimationStateMachine(owner_id, skeletal_mesh_id, range, bone_name) {};

	virtual void OnInit() override {
		states_.insert({ IDLE, make_shared<Idle>() });
		states_.insert({ MOVE, make_shared<Move>() });
		states_.insert({ HIT, make_shared<Hit>() });
		states_.insert({ DIE, make_shared<Die>() });
		states_.insert({ ATTACK, make_shared<Attack>() });
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
				if (enemy->is_attacking_ == true) {
					return true;
				}
				else {
					return false;
				}
			})
		});

		transitions_.insert({ IDLE, Transition(ATTACK,[this](const AnimationStateMachine* animation_state_machine) {
				Enemy* enemy = SCENE_MGR->GetActor<Enemy>(owner_id_);
				if (enemy->is_attacking_ == true) {
					return true;
				}
				else {
					return false;
				}
			})
		});

		transitions_.insert({ MOVE, Transition(ATTACK,[this](const AnimationStateMachine* animation_state_machine) {
				Enemy* enemy = SCENE_MGR->GetActor<Enemy>(owner_id_);
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

	class Attack : public AnimationState {
	public:
		Attack() : AnimationState(ATTACK) {}
	public:
		virtual void Enter(AnimationStateMachine* animation_state_machine) override
		{
			animation_state_machine->SetAnimation("Zombie_Atk_Arms_3_SHORT_Loop_IPC_Retargeted_Unreal Take.anim", 0.5f);
		}
		virtual void Exit(AnimationStateMachine* animation_state_machine) override
		{
			Enemy* enemy = SCENE_MGR->GetActor<Enemy>(animation_state_machine->GetOwnerId());
			enemy->is_attack_ended = true;
			enemy->is_attacking_ = false;
		}
		virtual void OnUpdate(AnimationStateMachine* animation_state_machine) override
		{
			Enemy* enemy = SCENE_MGR->GetActor<Enemy>(animation_state_machine->GetOwnerId());
			enemy->CancelMovement();
		}
	};
};