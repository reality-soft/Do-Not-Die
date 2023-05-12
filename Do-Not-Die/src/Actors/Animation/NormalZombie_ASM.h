#pragma once
#include "Engine_include.h"
#include "AnimationStateMachine.h"
#include "AnimationState.h"
#include "NormalZombie.h"
using namespace reality;

class NormalZombieBaseAnimationStateMachine : public AnimationStateMachine {
public:
	enum States {
		IDLE_BASE,
		MOVE_BASE,
		ATTACK_BASE,
		HIT_BASE,
	};

	NormalZombieBaseAnimationStateMachine(entt::entity owner_id, string skeletal_mesh_id, int range, string bone_name = "") : AnimationStateMachine(owner_id, skeletal_mesh_id, range, bone_name) {};

	virtual void OnInit() override {
		states_.insert({ IDLE_BASE, make_shared<Idle_Base>() });
		states_.insert({ MOVE_BASE, make_shared<Move_Base>() });
		states_.insert({ ATTACK_BASE, make_shared<Attack_Base>() });
		states_.insert({ HIT_BASE, make_shared<Hit_Base>() });

		transitions_.insert({ IDLE_BASE, Transition(MOVE_BASE,[this](const AnimationStateMachine* animation_state_machine) {
				NormalZombie* enemy = SCENE_MGR->GetActor<NormalZombie>(owner_id_);
				if (enemy->is_moving_) {
					return true;
				}
				else {
					return false;
				}
			})
			});
		transitions_.insert({ MOVE_BASE, Transition(ATTACK_BASE,[this](const AnimationStateMachine* animation_state_machine) {
				NormalZombie* enemy = SCENE_MGR->GetActor<NormalZombie>(owner_id_);
				if (enemy->is_attacking_) {
					return true;
				}
				else {
					return false;
				}
			})
			});
		transitions_.insert({ ATTACK_BASE, Transition(MOVE_BASE,[this](const AnimationStateMachine* animation_state_machine) {
				NormalZombie* enemy = SCENE_MGR->GetActor<NormalZombie>(owner_id_);
				if (enemy->is_attacking_ == false) {
					return true;
				}
				else {
					return false;
				}
			})
			});
		transitions_.insert({ MOVE_BASE, Transition(IDLE_BASE,[this](const AnimationStateMachine* animation_state_machine) {
				NormalZombie* enemy = SCENE_MGR->GetActor<NormalZombie>(owner_id_);
				if (enemy->is_moving_ == false) {
					return true;
				}
				else {
					return false;
				}
			})
			});
		transitions_.insert({ MOVE_BASE, Transition(HIT_BASE,[this](const AnimationStateMachine* animation_state_machine) {
				NormalZombie* enemy = SCENE_MGR->GetActor<NormalZombie>(owner_id_);
				if (enemy->is_hit_ && enemy->GetStatus("hp")->GetCurrentValue() > 0.0f) {
					return true;
				}
				else {
					return false;
				}
			})
			});
		transitions_.insert({ ATTACK_BASE, Transition(HIT_BASE,[this](const AnimationStateMachine* animation_state_machine) {
				NormalZombie* enemy = SCENE_MGR->GetActor<NormalZombie>(owner_id_);
				if (enemy->is_hit_ && enemy->GetStatus("hp")->GetCurrentValue() > 0.0f) {
					return true;
				}
				else {
					return false;
				}
			})
			});
		transitions_.insert({ IDLE_BASE, Transition(HIT_BASE,[this](const AnimationStateMachine* animation_state_machine) {
				NormalZombie* enemy = SCENE_MGR->GetActor<NormalZombie>(owner_id_);
				if (enemy->is_hit_) {
					return true;
				}
				else {
					return false;
				}
			})
			});
		transitions_.insert({ HIT_BASE, Transition(HIT_BASE,[this](const AnimationStateMachine* animation_state_machine) {
				NormalZombie* enemy = SCENE_MGR->GetActor<NormalZombie>(owner_id_);
				if (enemy->is_hit_ == true && enemy->GetStatus("hp")->GetCurrentValue() > 0.0f) {
					return true;
				}
				else {
					return false;
				}
			})
			});
		transitions_.insert({ HIT_BASE, Transition(IDLE_BASE,[this](const AnimationStateMachine* animation_state_machine) {
				NormalZombie* enemy = SCENE_MGR->GetActor<NormalZombie>(owner_id_);
				if (IsAnimationEnded() && enemy->GetStatus("hp")->GetCurrentValue() > 0.0f && enemy->is_moving_ == false) {
					return true;
				}
				else {
					return false;
				}
			})
			});
		transitions_.insert({ HIT_BASE, Transition(MOVE_BASE,[this](const AnimationStateMachine* animation_state_machine) {
				NormalZombie* enemy = SCENE_MGR->GetActor<NormalZombie>(owner_id_);
				if (IsAnimationEnded() && enemy->GetStatus("hp")->GetCurrentValue() > 0 && enemy->is_moving_ == true) {
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
			animation_state_machine->SetAnimation("DND_Idle_1.anim", 0.3f, true);
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
			selected_chase_anim = RandomIntInRange(0, 4);
			animation_state_machine->SetAnimation(chase_animations_[selected_chase_anim], 0.8f, true, notifies);
		}
		virtual void Exit(AnimationStateMachine* animation_state_machine) override
		{
		}
		virtual void OnUpdate(AnimationStateMachine* animation_state_machine) override
		{
		}

	private:
		vector<string> chase_animations_ = { "DND_HyperChase1.anim",
											 "DND_HyperChase2.anim",
											 "DND_HyperChase3.anim",
											 "DND_HyperChase4.anim",
											 "DND_HyperChase5.anim" };
											 
		UINT selected_chase_anim = 0;
	};

	class Attack_Base : public AnimationState {
	public:
		Attack_Base() : AnimationState(ATTACK_BASE) {}

	public:
		virtual void Enter(AnimationStateMachine* animation_state_machine) override
		{
			animation_state_machine->SetAnimation("DND_NormalChase.anim", 0.3f, true);
		}
		virtual void Exit(AnimationStateMachine* animation_state_machine) override
		{
			NormalZombie* enemy = SCENE_MGR->GetActor<NormalZombie>(animation_state_machine->GetOwnerId());
			enemy->is_attacking_ = false;
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
			animation_state_machine->SetAnimation("DND_KnockBack_1.anim", 0.8f, true);
			SCENE_MGR->GetActor<NormalZombie>(animation_state_machine->GetOwnerId())->is_hit_ = false;
		}
		virtual void Exit(AnimationStateMachine* animation_state_machine) override
		{
		}
		virtual void OnUpdate(AnimationStateMachine* animation_state_machine) override
		{
			NormalZombie* enemy = SCENE_MGR->GetActor<NormalZombie>(animation_state_machine->GetOwnerId());
			enemy->CancelMovement();
		}
	};

	//class Die_Base : public AnimationState {
	//public:
	//	Die_Base() : AnimationState(DIE_BASE) {}
	//public:
	//	virtual void Enter(AnimationStateMachine* animation_state_machine) override
	//	{
	//		EVENT->PushEvent<SoundGenerateEvent>(animation_state_machine->GetOwnerId(), SFX, "ZombieDie_1.wav", 0.5f, false);
	//		NormalZombie* enemy = SCENE_MGR->GetActor<NormalZombie>(animation_state_machine->GetOwnerId());
	//		enemy->GetCapsuleComponent()->capsule.height = 3.f;
	//		enemy->GetCapsuleComponent()->capsule.radius = 3.f;
	//		animation_state_machine->SetAnimation("DND_Death_1.anim", 0.5f, true);
	//	}
	//	virtual void Exit(AnimationStateMachine* animation_state_machine) override
	//	{
	//		EVENT->PushEvent<KillEvent>();
	//		EVENT->PushEvent<DeleteActorEvent>(animation_state_machine->GetOwnerId());

	//	}
	//	virtual void OnUpdate(AnimationStateMachine* animation_state_machine) override
	//	{
	//		NormalZombie* enemy = SCENE_MGR->GetActor<NormalZombie>(animation_state_machine->GetOwnerId());
	//		enemy->CancelMovement();
	//	}
	//};
};

class NormalZombieUpperBodyAnimationStateMachine : public AnimationStateMachine {
public:
	enum States {
		IDLE,
		MOVE,
		HIT,
		DIE,
		ATTACK
	};

	NormalZombieUpperBodyAnimationStateMachine(entt::entity owner_id, string skeletal_mesh_id, int range, string bone_name = "") : AnimationStateMachine(owner_id, skeletal_mesh_id, range, bone_name) {};

	virtual void OnInit() override {
		states_.insert({ IDLE, make_shared<Idle>() });
		states_.insert({ MOVE, make_shared<Move>() });
		states_.insert({ HIT, make_shared<Hit>() });
		states_.insert({ DIE, make_shared<Die>() });
		states_.insert({ ATTACK, make_shared<Attack>() });
		transitions_.insert({ IDLE, Transition(MOVE,[this](const AnimationStateMachine* animation_state_machine) {
				NormalZombie* enemy = SCENE_MGR->GetActor<NormalZombie>(owner_id_);
				if (enemy->is_moving_) {
					return true;
				}
				else {
					return false;
				}
			})
		});
		transitions_.insert({ MOVE, Transition(IDLE,[this](const AnimationStateMachine* animation_state_machine) {
				NormalZombie* enemy = SCENE_MGR->GetActor<NormalZombie>(owner_id_);
				if (enemy->is_moving_ == false) {
					return true;
				}
				else {
					return false;
				}
			})
		});
		transitions_.insert({ MOVE, Transition(HIT,[this](const AnimationStateMachine* animation_state_machine) {
				NormalZombie* enemy = SCENE_MGR->GetActor<NormalZombie>(owner_id_);
				if (enemy->is_hit_ && enemy->GetStatus("hp")->GetCurrentValue() > 0.0f) {
					return true;
				}
				else {
					return false;
				}
			})
		});
		transitions_.insert({ IDLE, Transition(HIT,[this](const AnimationStateMachine* animation_state_machine) {
				NormalZombie* enemy = SCENE_MGR->GetActor<NormalZombie>(owner_id_);
				if (enemy->is_hit_) {
					return true;
				}
				else {
					return false;
				}
			})
		});
		transitions_.insert({ HIT, Transition(HIT,[this](const AnimationStateMachine* animation_state_machine) {
				NormalZombie* enemy = SCENE_MGR->GetActor<NormalZombie>(owner_id_);
				if (enemy->is_hit_ == true && enemy->GetStatus("hp")->GetCurrentValue() > 0.0f) {
					return true;
				}
				else {
					return false;
				}
			})
		});
		transitions_.insert({ HIT, Transition(IDLE,[this](const AnimationStateMachine* animation_state_machine) {
				NormalZombie* enemy = SCENE_MGR->GetActor<NormalZombie>(owner_id_);
				if (IsAnimationEnded() && enemy->GetStatus("hp")->GetCurrentValue() > 0.0f && enemy->is_moving_ == false) {
					return true;
				}
				else {
					return false;
				}
			})
		});
		transitions_.insert({ HIT, Transition(MOVE,[this](const AnimationStateMachine* animation_state_machine) {
				NormalZombie* enemy = SCENE_MGR->GetActor<NormalZombie>(owner_id_);
				if (IsAnimationEnded() && enemy->GetStatus("hp")->GetCurrentValue() > 0 && enemy->is_moving_ == true) {
					return true;
				}
				else {
					return false;
				}
			})
		});

		transitions_.insert({ HIT, Transition(DIE,[this](const AnimationStateMachine* animation_state_machine) {
				NormalZombie* enemy = SCENE_MGR->GetActor<NormalZombie>(owner_id_);
				if (enemy->GetStatus("hp")->GetCurrentValue() <= 0.0f) {
					return true;
				}
				else {
					return false;
				}
			})
		});
		transitions_.insert({ MOVE, Transition(DIE,[this](const AnimationStateMachine* animation_state_machine) {
				NormalZombie* enemy = SCENE_MGR->GetActor<NormalZombie>(owner_id_);
				if (enemy->GetStatus("hp")->GetCurrentValue() <= 0.0f) {
					return true;
				}
				else {
					return false;
				}
			})
		});
		transitions_.insert({ IDLE, Transition(DIE,[this](const AnimationStateMachine* animation_state_machine) {
				NormalZombie* enemy = SCENE_MGR->GetActor<NormalZombie>(owner_id_);
				if (enemy->GetStatus("hp")->GetCurrentValue() <= 0.0f) {
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
				NormalZombie* enemy = SCENE_MGR->GetActor<NormalZombie>(owner_id_);
				if (enemy->is_attacking_ == true) {
					return true;
				}
				else {
					return false;
				}
			})
		});

		transitions_.insert({ MOVE, Transition(ATTACK,[this](const AnimationStateMachine* animation_state_machine) {
				NormalZombie* enemy = SCENE_MGR->GetActor<NormalZombie>(owner_id_);
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
		int combo_ = 1;
	public:
		virtual void Enter(AnimationStateMachine* animation_state_machine) override
		{
			combo_ = 1;
			EVENT->PushEvent<SoundGenerateEvent>(animation_state_machine->GetOwnerId(), SFX, "ZombieAttack_1.mp3", 0.5f, false);
			animation_state_machine->SetAnimation("DND_Attack_1.anim", 0.5f, true);						
			SCENE_MGR->GetActor<NormalZombie>(animation_state_machine->GetOwnerId())->GetStatus("max_speed")->SetDefualtValue(50);
		}
		virtual void Exit(AnimationStateMachine* animation_state_machine) override
		{
			NormalZombie* enemy = SCENE_MGR->GetActor<NormalZombie>(animation_state_machine->GetOwnerId());
			enemy->is_attack_ended_ = true;
			enemy->is_attacking_ = false;
			SCENE_MGR->GetActor<NormalZombie>(animation_state_machine->GetOwnerId())->GetStatus("max_speed")->SetDefualtValue(RandomIntInRange(100, 200));
			combo_ = 0;
		}
		virtual void OnUpdate(AnimationStateMachine* animation_state_machine) override
		{
			NormalZombie* enemy = SCENE_MGR->GetActor<NormalZombie>(animation_state_machine->GetOwnerId());
			if (combo_ > 0 && animation_state_machine->GetCurAnimation().cur_frame_ >= 20.0f)
			{
				auto c_enemy_capsule = SCENE_MGR->GetScene(INGAME)->GetRegistryRef().try_get<C_CapsuleCollision>(enemy->GetEntityId());
				if (c_enemy_capsule == nullptr) return;

				RayShape attack_ray;
				attack_ray.start = _XMFLOAT3(GetTipBaseAB(c_enemy_capsule->capsule)[3]);
				attack_ray.end = _XMFLOAT3((_XMVECTOR3(attack_ray.start) + (enemy->GetFront() * enemy->attack_distance_)));
				EVENT->PushEvent<AttackEvent_SingleRay>(attack_ray, enemy->GetEntityId());
				EVENT->PushEvent<AttackEvent_AboutCar>(enemy->GetEntityId());
				combo_--;
			}
		}
	};
};

class NormalZombieFullBodyAnimationStateMachine : public AnimationStateMachine {
public:
	enum States {
		NONE,
		DIE,

	};

	NormalZombieFullBodyAnimationStateMachine(entt::entity owner_id, string skeletal_mesh_id, int range, string bone_name = "") : AnimationStateMachine(owner_id, skeletal_mesh_id, range, bone_name) {};

	virtual void OnInit() override {
		states_.insert({ NONE, make_shared<None>() });
		states_.insert({ DIE, make_shared<Die>() });


		// Die
		{
			transitions_.insert({ NONE, Transition(DIE,[this](const AnimationStateMachine* animation_state_machine) {
				entt::entity owner_id = animation_state_machine->GetOwnerId();
				NormalZombie* enemy = SCENE_MGR->GetActor<NormalZombie>(owner_id);
				if (enemy->GetStatus("hp")->GetCurrentValue() <= 0.001f) {
					return true;
				}
				else {
					return false;
				}
			})
			});
			transitions_.insert({ DIE, Transition(NONE,[this](const AnimationStateMachine* animation_state_machine) {
				if (IsAnimationEnded()) {
					return true;
				}
				else {
					return false;
				}
			})
			});
		}

		cur_state_ = states_[NONE];
	}

	class None : public AnimationState {
	public:
		None() : AnimationState(NONE) {}
	public:
		virtual void Enter(AnimationStateMachine* animation_state_machine) override
		{
			animation_state_machine->SetAnimation("", 0.2f, true);
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
			EVENT->PushEvent<SoundGenerateEvent>(animation_state_machine->GetOwnerId(), SFX, "ZombieDie_1.wav", 0.5f, false);
			NormalZombie* enemy = SCENE_MGR->GetActor<NormalZombie>(animation_state_machine->GetOwnerId());
			enemy->GetCapsuleComponent()->capsule.height = 3.f;
			enemy->GetCapsuleComponent()->capsule.radius = 3.f;
			animation_state_machine->SetAnimation("DND_Death_1.anim", 0.5f, false);
		}
		virtual void Exit(AnimationStateMachine* animation_state_machine) override
		{
			EVENT->PushEvent<KillEvent>();
			EVENT->PushEvent<DeleteActorEvent>(animation_state_machine->GetOwnerId());
		}
		virtual void OnUpdate(AnimationStateMachine* animation_state_machine) override
		{
			NormalZombie* enemy = SCENE_MGR->GetActor<NormalZombie>(animation_state_machine->GetOwnerId());
			enemy->CancelMovement();
		}
	};

};

