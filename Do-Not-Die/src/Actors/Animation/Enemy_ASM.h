#pragma once
#include "Engine_include.h"
#include "AnimationStateMachine.h"
#include "AnimationState.h"
#include "GeneralZombie.h"
using namespace reality;

class ZombieBaseAnimationStateMachine : public AnimationStateMachine {
public:
	enum States {
		IDLE_BASE,
		MOVE_BASE,
		HIT_BASE,
		DIE_BASE,
	};

	ZombieBaseAnimationStateMachine(entt::entity owner_id, string skeletal_mesh_id, int range, string bone_name = "") : AnimationStateMachine(owner_id, skeletal_mesh_id, range, bone_name) {};

	virtual void OnInit() override {
		states_.insert({ IDLE_BASE, make_shared<Idle_Base>() });
		states_.insert({ MOVE_BASE, make_shared<Move_Base>() });
		states_.insert({ HIT_BASE, make_shared<Hit_Base>() });
		states_.insert({ DIE_BASE, make_shared<Die_Base>() });
		transitions_.insert({ IDLE_BASE, Transition(MOVE_BASE,[this](const AnimationStateMachine* animation_state_machine) {
				GeneralZombie* enemy = SCENE_MGR->GetActor<GeneralZombie>(owner_id_);
				if (enemy->IsMoving()) {
					return true;
				}
				else {
					return false;
				}
			})
			});
		transitions_.insert({ MOVE_BASE, Transition(IDLE_BASE,[this](const AnimationStateMachine* animation_state_machine) {
				GeneralZombie* enemy = SCENE_MGR->GetActor<GeneralZombie>(owner_id_);
				if (enemy->IsMoving() == false) {
					return true;
				}
				else {
					return false;
				}
			})
			});
		transitions_.insert({ MOVE_BASE, Transition(HIT_BASE,[this](const AnimationStateMachine* animation_state_machine) {
				GeneralZombie* enemy = SCENE_MGR->GetActor<GeneralZombie>(owner_id_);
				if (enemy->is_hit_ && enemy->GetCurHp() > 0.0f) {
					return true;
				}
				else {
					return false;
				}
			})
			});
		transitions_.insert({ IDLE_BASE, Transition(HIT_BASE,[this](const AnimationStateMachine* animation_state_machine) {
				GeneralZombie* enemy = SCENE_MGR->GetActor<GeneralZombie>(owner_id_);
				if (enemy->is_hit_) {
					return true;
				}
				else {
					return false;
				}
			})
			});
		transitions_.insert({ HIT_BASE, Transition(HIT_BASE,[this](const AnimationStateMachine* animation_state_machine) {
				GeneralZombie* enemy = SCENE_MGR->GetActor<GeneralZombie>(owner_id_);
				if (enemy->is_hit_ == true && enemy->GetCurHp() > 0.0f) {
					return true;
				}
				else {
					return false;
				}
			})
			});
		transitions_.insert({ HIT_BASE, Transition(IDLE_BASE,[this](const AnimationStateMachine* animation_state_machine) {
				GeneralZombie* enemy = SCENE_MGR->GetActor<GeneralZombie>(owner_id_);
				if (IsAnimationEnded() && enemy->GetCurHp() > 0.0f && enemy->IsMoving() == false) {
					return true;
				}
				else {
					return false;
				}
			})
			});
		transitions_.insert({ HIT_BASE, Transition(MOVE_BASE,[this](const AnimationStateMachine* animation_state_machine) {
				GeneralZombie* enemy = SCENE_MGR->GetActor<GeneralZombie>(owner_id_);
				if (IsAnimationEnded() && enemy->GetCurHp() > 0 && enemy->IsMoving() == true) {
					return true;
				}
				else {
					return false;
				}
			})
			});

		transitions_.insert({ HIT_BASE, Transition(DIE_BASE,[this](const AnimationStateMachine* animation_state_machine) {
				GeneralZombie* enemy = SCENE_MGR->GetActor<GeneralZombie>(owner_id_);
				if (enemy->GetCurHp() <= 0.0f) {
					return true;
				}
				else {
					return false;
				}
			})
			});
		transitions_.insert({ MOVE_BASE, Transition(DIE_BASE,[this](const AnimationStateMachine* animation_state_machine) {
				GeneralZombie* enemy = SCENE_MGR->GetActor<GeneralZombie>(owner_id_);
				if (enemy->GetCurHp() <= 0.0f) {
					return true;
				}
				else {
					return false;
				}
			})
			});
		transitions_.insert({ IDLE_BASE, Transition(DIE_BASE,[this](const AnimationStateMachine* animation_state_machine) {
				GeneralZombie* enemy = SCENE_MGR->GetActor<GeneralZombie>(owner_id_);
				if (enemy->GetCurHp() <= 0.0f) {
					return true;
				}
				else {
					return false;
				}
			})
			});

		transitions_.insert({ DIE_BASE, Transition(IDLE_BASE,[this](const AnimationStateMachine* animation_state_machine) {
				if (IsAnimationEnded() == true) {
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
			animation_state_machine->SetAnimation("Zombie_Idle_1_v2_IPC_Anim.anim", 0.3f, true);
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
			animation_state_machine->SetAnimation("Zombie_Walk_F_6_Loop_IPC_Anim_Unreal Take.anim", 0.8f, true, notifies);
		}
		virtual void Exit(AnimationStateMachine* animation_state_machine) override
		{
		}
		virtual void OnUpdate(AnimationStateMachine* animation_state_machine) override
		{
		}
	};


	class Hit_Base : public AnimationState {
	public:
		Hit_Base() : AnimationState(HIT_BASE) {}
	public:
		virtual void Enter(AnimationStateMachine* animation_state_machine) override
		{
			EVENT->PushEvent<SoundGenerateEvent>(animation_state_machine->GetOwnerId(), SFX, "ZombieHit_1.wav", 0.5f, false);
			animation_state_machine->SetAnimation("Zombie_Atk_KnockBack_1_IPC_Anim_Unreal Take.anim", 0.8f, true);
			SCENE_MGR->GetActor<GeneralZombie>(animation_state_machine->GetOwnerId())->is_hit_ = false;
		}
		virtual void Exit(AnimationStateMachine* animation_state_machine) override
		{
		}
		virtual void OnUpdate(AnimationStateMachine* animation_state_machine) override
		{
			GeneralZombie* enemy = SCENE_MGR->GetActor<GeneralZombie>(animation_state_machine->GetOwnerId());
			enemy->CancelMovement();
		}
	};

	class Die_Base : public AnimationState {
	public:
		Die_Base() : AnimationState(DIE_BASE) {}
	public:
		virtual void Enter(AnimationStateMachine* animation_state_machine) override
		{
			EVENT->PushEvent<SoundGenerateEvent>(animation_state_machine->GetOwnerId(), SFX, "ZombieDie_1.wav", 0.5f, false);
			GeneralZombie* enemy = SCENE_MGR->GetActor<GeneralZombie>(animation_state_machine->GetOwnerId());
			enemy->GetCapsuleComponent()->capsule.height = 3.f;
			enemy->GetCapsuleComponent()->capsule.radius = 3.f;
			animation_state_machine->SetAnimation("Zombie_Death_Back_Mid_1_IPC_Anim_Unreal Take.anim", 0.5f, true);
		}
		virtual void Exit(AnimationStateMachine* animation_state_machine) override
		{
			EVENT->PushEvent<KillEvent>();
			EVENT->PushEvent<DeleteActorEvent>(animation_state_machine->GetOwnerId());

		}
		virtual void OnUpdate(AnimationStateMachine* animation_state_machine) override
		{
			GeneralZombie* enemy = SCENE_MGR->GetActor<GeneralZombie>(animation_state_machine->GetOwnerId());
			enemy->CancelMovement();
		}
	};
};

class ZombieUpperBodyAnimationStateMachine : public AnimationStateMachine {
public:
	enum States {
		IDLE,
		MOVE,
		HIT,
		DIE,
		ATTACK
	};

	ZombieUpperBodyAnimationStateMachine(entt::entity owner_id, string skeletal_mesh_id, int range, string bone_name = "") : AnimationStateMachine(owner_id, skeletal_mesh_id, range, bone_name) {};

	virtual void OnInit() override {
		states_.insert({ IDLE, make_shared<Idle>() });
		states_.insert({ MOVE, make_shared<Move>() });
		states_.insert({ HIT, make_shared<Hit>() });
		states_.insert({ DIE, make_shared<Die>() });
		states_.insert({ ATTACK, make_shared<Attack>() });
		transitions_.insert({ IDLE, Transition(MOVE,[this](const AnimationStateMachine* animation_state_machine) {
				GeneralZombie* enemy = SCENE_MGR->GetActor<GeneralZombie>(owner_id_);
				if (enemy->IsMoving()) {
					return true;
				}
				else {
					return false;
				}
			})
		});
		transitions_.insert({ MOVE, Transition(IDLE,[this](const AnimationStateMachine* animation_state_machine) {
				GeneralZombie* enemy = SCENE_MGR->GetActor<GeneralZombie>(owner_id_);
				if (enemy->IsMoving() == false) {
					return true;
				}
				else {
					return false;
				}
			})
		});
		transitions_.insert({ MOVE, Transition(HIT,[this](const AnimationStateMachine* animation_state_machine) {
				GeneralZombie* enemy = SCENE_MGR->GetActor<GeneralZombie>(owner_id_);
				if (enemy->is_hit_ && enemy->GetCurHp() > 0.0f) {
					return true;
				}
				else {
					return false;
				}
			})
		});
		transitions_.insert({ IDLE, Transition(HIT,[this](const AnimationStateMachine* animation_state_machine) {
				GeneralZombie* enemy = SCENE_MGR->GetActor<GeneralZombie>(owner_id_);
				if (enemy->is_hit_) {
					return true;
				}
				else {
					return false;
				}
			})
		});
		transitions_.insert({ HIT, Transition(HIT,[this](const AnimationStateMachine* animation_state_machine) {
				GeneralZombie* enemy = SCENE_MGR->GetActor<GeneralZombie>(owner_id_);
				if (enemy->is_hit_ == true && enemy->GetCurHp() > 0.0f) {
					return true;
				}
				else {
					return false;
				}
			})
		});
		transitions_.insert({ HIT, Transition(IDLE,[this](const AnimationStateMachine* animation_state_machine) {
				GeneralZombie* enemy = SCENE_MGR->GetActor<GeneralZombie>(owner_id_);
				if (IsAnimationEnded() && enemy->GetCurHp() > 0.0f && enemy->IsMoving() == false) {
					return true;
				}
				else {
					return false;
				}
			})
		});
		transitions_.insert({ HIT, Transition(MOVE,[this](const AnimationStateMachine* animation_state_machine) {
				GeneralZombie* enemy = SCENE_MGR->GetActor<GeneralZombie>(owner_id_);
				if (IsAnimationEnded() && enemy->GetCurHp() > 0 && enemy->IsMoving() == true) {
					return true;
				}
				else {
					return false;
				}
			})
		});

		transitions_.insert({ HIT, Transition(DIE,[this](const AnimationStateMachine* animation_state_machine) {
				GeneralZombie* enemy = SCENE_MGR->GetActor<GeneralZombie>(owner_id_);
				if (enemy->GetCurHp() <= 0.0f) {
					return true;
				}
				else {
					return false;
				}
			})
		});
		transitions_.insert({ MOVE, Transition(DIE,[this](const AnimationStateMachine* animation_state_machine) {
				GeneralZombie* enemy = SCENE_MGR->GetActor<GeneralZombie>(owner_id_);
				if (enemy->GetCurHp() <= 0.0f) {
					return true;
				}
				else {
					return false;
				}
			})
		});
		transitions_.insert({ IDLE, Transition(DIE,[this](const AnimationStateMachine* animation_state_machine) {
				GeneralZombie* enemy = SCENE_MGR->GetActor<GeneralZombie>(owner_id_);
				if (enemy->GetCurHp() <= 0.0f) {
					return true;
				}
				else {
					return false;
				}
			})
		});

		transitions_.insert({ DIE, Transition(IDLE,[this](const AnimationStateMachine* animation_state_machine) {
				if (IsAnimationEnded() == true) {
					return true;
				}
				else {
					return false;
				}
			})
		});

		transitions_.insert({ IDLE, Transition(ATTACK,[this](const AnimationStateMachine* animation_state_machine) {
				GeneralZombie* enemy = SCENE_MGR->GetActor<GeneralZombie>(owner_id_);
				if (enemy->is_attacking_ == true) {
					return true;
				}
				else {
					return false;
				}
			})
		});

		transitions_.insert({ MOVE, Transition(ATTACK,[this](const AnimationStateMachine* animation_state_machine) {
				GeneralZombie* enemy = SCENE_MGR->GetActor<GeneralZombie>(owner_id_);
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
			animation_state_machine->SetAnimation("", 0.3f, true);
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
			animation_state_machine->SetAnimation("", 0.8f, true);
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
			animation_state_machine->SetAnimation("", 0.8f, true);
		}
		virtual void Exit(AnimationStateMachine* animation_state_machine) override
		{
		}
		virtual void OnUpdate(AnimationStateMachine* animation_state_machine) override
		{
		}
	};

	class Die : public AnimationState {
	public:
		Die() : AnimationState(DIE) {}
	public:
		virtual void Enter(AnimationStateMachine* animation_state_machine) override
		{
			animation_state_machine->SetAnimation("", 0.5f, true);
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
			animation_state_machine->SetAnimation("Zombie_Atk_Arms_3_SHORT_Loop_IPC_Retargeted_Unreal Take.anim", 0.5f, true);
		}
		virtual void Exit(AnimationStateMachine* animation_state_machine) override
		{
			GeneralZombie* enemy = SCENE_MGR->GetActor<GeneralZombie>(animation_state_machine->GetOwnerId());
			enemy->is_attack_ended = true;
			enemy->is_attacking_ = false;
		}
		virtual void OnUpdate(AnimationStateMachine* animation_state_machine) override
		{
			GeneralZombie* enemy = SCENE_MGR->GetActor<GeneralZombie>(animation_state_machine->GetOwnerId());
			//enemy->CancelMovement();
		}
	};
};