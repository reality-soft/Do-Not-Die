#pragma once
#include "Engine_include.h"
#include "AnimationStateMachine.h"
#include "AnimationState.h"
#include "Player.h"
using namespace reality;

class PlayerUpperBodyAnimationStateMachine : public AnimationStateMachine {
public:
	enum States {
		IDLE_POSE_AR,
		IDLE_POSE_HG,
		IDLE_POSE_GR,
		AIM_POSE_AR,
		AIM_POSE_HG,
		AIM_POSE_MELEE,
		AIM_POSE_GR,
		ATTACK_AR,
		ATTACK_HG,
		ATTACK_MELEE,
		ATTACK_GR,
		HIT,
		DIE
	};

	PlayerUpperBodyAnimationStateMachine(entt::entity owner_id, string skeletal_mesh_id, int range, string bone_name = "") : AnimationStateMachine(owner_id, skeletal_mesh_id, range, bone_name) {};

	virtual void OnInit() override {
		states_.insert({ IDLE_POSE_AR, make_shared<IdlePoseAR>() });
		states_.insert({ IDLE_POSE_HG, make_shared<IdlePoseHG>() });
		states_.insert({ IDLE_POSE_GR, make_shared<IdlePoseGR>() });
		states_.insert({ AIM_POSE_AR, make_shared<AimPoseAR>() });
		states_.insert({ AIM_POSE_HG, make_shared<AimPoseHG>() });
		states_.insert({ AIM_POSE_MELEE, make_shared<AimPoseMelee>() });
		states_.insert({ AIM_POSE_GR, make_shared<AimPoseGR>() });
		states_.insert({ ATTACK_AR, make_shared<AttackAR>() });
		states_.insert({ ATTACK_MELEE, make_shared<AttackMelee>() });
		states_.insert({ ATTACK_HG, make_shared<AttackHG>() });
		states_.insert({ ATTACK_GR, make_shared<AttackGR>() });
		states_.insert({ HIT, make_shared<Hit>() });
		states_.insert({ DIE, make_shared<Die>() });

		// Auto Rifle
		{
			// Pose Switch
			{
				transitions_.insert({ IDLE_POSE_AR, Transition(IDLE_POSE_HG,[this](const AnimationStateMachine* animation_state_machine) {
						entt::entity owner_id = animation_state_machine->GetOwnerId();
						Player* player = SCENE_MGR->GetActor<Player>(owner_id);
						if (player->cur_equipped_weapon_ == EQUIPPED_WEAPON::HAND_GUN) {
							return true;
						}
						else {
							return false;
						}
					})
					});
				transitions_.insert({ IDLE_POSE_AR, Transition(AIM_POSE_MELEE,[this](const AnimationStateMachine* animation_state_machine) {
						entt::entity owner_id = animation_state_machine->GetOwnerId();
						Player* player = SCENE_MGR->GetActor<Player>(owner_id);
						if (player->cur_equipped_weapon_ == EQUIPPED_WEAPON::MELEE_WEAPON) {
							return true;
						}
						else {
							return false;
						}
					})
					});
				transitions_.insert({ IDLE_POSE_AR, Transition(IDLE_POSE_GR,[this](const AnimationStateMachine* animation_state_machine) {
						entt::entity owner_id = animation_state_machine->GetOwnerId();
						Player* player = SCENE_MGR->GetActor<Player>(owner_id);
						if (player->cur_equipped_weapon_ == EQUIPPED_WEAPON::GRENADE) {
							return true;
						}
						else {
							return false;
						}
					})
					});
			}

			// Aim
			{
				transitions_.insert({ IDLE_POSE_AR, Transition(AIM_POSE_AR,[this](const AnimationStateMachine* animation_state_machine) {
						entt::entity owner_id = animation_state_machine->GetOwnerId();
						Player* player = SCENE_MGR->GetActor<Player>(owner_id);
						if (player->IsAiming() == true) {
							return true;
						}
						else {
							return false;
						}
					})
					});
				transitions_.insert({ AIM_POSE_AR, Transition(IDLE_POSE_AR,[this](const AnimationStateMachine* animation_state_machine) {
						entt::entity owner_id = animation_state_machine->GetOwnerId();
						Player* player = SCENE_MGR->GetActor<Player>(owner_id);
						if (player->IsAiming() == false) {
							return true;
						}
						else {
							return false;
						}
					})
					});
			}

			// Attack
			{
				transitions_.insert({ AIM_POSE_AR, Transition(ATTACK_AR,[this](const AnimationStateMachine* animation_state_machine) {
						entt::entity owner_id = animation_state_machine->GetOwnerId();
						Player* player = SCENE_MGR->GetActor<Player>(owner_id);
						if (player->is_firing_ == true) {
							return true;
						}
						else {
							return false;
						}
					})
					});
				transitions_.insert({ ATTACK_AR, Transition(AIM_POSE_AR,[this](const AnimationStateMachine* animation_state_machine) {
						if (IsAnimationEnded()) {
							return true;
						}
						else {
							return false;
						}
					})
					});
			}
		}

		// Hand Gun
		{
			// Pose Switch
			{
				transitions_.insert({ IDLE_POSE_HG, Transition(IDLE_POSE_AR,[this](const AnimationStateMachine* animation_state_machine) {
						entt::entity owner_id = animation_state_machine->GetOwnerId();
						Player* player = SCENE_MGR->GetActor<Player>(owner_id);
						if (player->cur_equipped_weapon_ == EQUIPPED_WEAPON::AUTO_RIFLE) {
							return true;
						}
						else {
							return false;
						}
					})
					});
				transitions_.insert({ IDLE_POSE_HG, Transition(AIM_POSE_MELEE,[this](const AnimationStateMachine* animation_state_machine) {
						entt::entity owner_id = animation_state_machine->GetOwnerId();
						Player* player = SCENE_MGR->GetActor<Player>(owner_id);
						if (player->cur_equipped_weapon_ == EQUIPPED_WEAPON::MELEE_WEAPON) {
							return true;
						}
						else {
							return false;
						}
					})
					});
				transitions_.insert({ IDLE_POSE_HG, Transition(IDLE_POSE_GR,[this](const AnimationStateMachine* animation_state_machine) {
						entt::entity owner_id = animation_state_machine->GetOwnerId();
						Player* player = SCENE_MGR->GetActor<Player>(owner_id);
						if (player->cur_equipped_weapon_ == EQUIPPED_WEAPON::GRENADE) {
							return true;
						}
						else {
							return false;
						}
					})
					});
			}

			// Aim
			{
				transitions_.insert({ IDLE_POSE_HG, Transition(AIM_POSE_HG,[this](const AnimationStateMachine* animation_state_machine) {
						entt::entity owner_id = animation_state_machine->GetOwnerId();
						Player* player = SCENE_MGR->GetActor<Player>(owner_id);
						if (player->IsAiming() == true) {
							return true;
						}
						else {
							return false;
						}
					})
					});
				transitions_.insert({ AIM_POSE_HG, Transition(IDLE_POSE_HG,[this](const AnimationStateMachine* animation_state_machine) {
						entt::entity owner_id = animation_state_machine->GetOwnerId();
						Player* player = SCENE_MGR->GetActor<Player>(owner_id);
						if (player->IsAiming() == false) {
							return true;
						}
						else {
							return false;
						}
					})
					});
			}

			// Attack
			{
				transitions_.insert({ AIM_POSE_HG, Transition(ATTACK_HG,[this](const AnimationStateMachine* animation_state_machine) {
						entt::entity owner_id = animation_state_machine->GetOwnerId();
						Player* player = SCENE_MGR->GetActor<Player>(owner_id);
						if (player->is_firing_ == true) {
							return true;
						}
						else {
							return false;
						}
					})
					});
				transitions_.insert({ ATTACK_HG, Transition(AIM_POSE_HG,[this](const AnimationStateMachine* animation_state_machine) {
						if (IsAnimationEnded()) {
							return true;
						}
						else {
							return false;
						}
					})
					});
			}
		}

		// Melee Weapon
		{
			// Pose Switch
			{
				transitions_.insert({ AIM_POSE_MELEE, Transition(IDLE_POSE_AR,[this](const AnimationStateMachine* animation_state_machine) {
						entt::entity owner_id = animation_state_machine->GetOwnerId();
						Player* player = SCENE_MGR->GetActor<Player>(owner_id);
						if (player->cur_equipped_weapon_ == EQUIPPED_WEAPON::AUTO_RIFLE) {
							return true;
						}
						else {
							return false;
						}
					})
					});
				transitions_.insert({ AIM_POSE_MELEE, Transition(AIM_POSE_HG,[this](const AnimationStateMachine* animation_state_machine) {
						entt::entity owner_id = animation_state_machine->GetOwnerId();
						Player* player = SCENE_MGR->GetActor<Player>(owner_id);
						if (player->cur_equipped_weapon_ == EQUIPPED_WEAPON::HAND_GUN) {
							return true;
						}
						else {
							return false;
						}
					})
					});
				transitions_.insert({ AIM_POSE_MELEE, Transition(IDLE_POSE_GR,[this](const AnimationStateMachine* animation_state_machine) {
						entt::entity owner_id = animation_state_machine->GetOwnerId();
						Player* player = SCENE_MGR->GetActor<Player>(owner_id);
						if (player->cur_equipped_weapon_ == EQUIPPED_WEAPON::GRENADE) {
							return true;
						}
						else {
							return false;
						}
					})
					});
			}

			// Attack
			{
				transitions_.insert({ AIM_POSE_MELEE, Transition(ATTACK_MELEE,[this](const AnimationStateMachine* animation_state_machine) {
						entt::entity owner_id = animation_state_machine->GetOwnerId();
						Player* player = SCENE_MGR->GetActor<Player>(owner_id);
						if (player->is_firing_ == true) {
							return true;
						}
						else {
							return false;
						}
					})
					});
				transitions_.insert({ ATTACK_MELEE, Transition(AIM_POSE_MELEE,[this](const AnimationStateMachine* animation_state_machine) {
						if (IsAnimationEnded()) {
							return true;
						}
						else {
							return false;
						}
					})
					});
			}
		}

		// Grenade
		{
			// Pose Switch
			{
				transitions_.insert({ AIM_POSE_GR, Transition(IDLE_POSE_AR,[this](const AnimationStateMachine* animation_state_machine) {
						entt::entity owner_id = animation_state_machine->GetOwnerId();
						Player* player = SCENE_MGR->GetActor<Player>(owner_id);
						if (player->cur_equipped_weapon_ == EQUIPPED_WEAPON::AUTO_RIFLE) {
							return true;
						}
						else {
							return false;
						}
					})
					});
				transitions_.insert({ AIM_POSE_GR, Transition(AIM_POSE_HG,[this](const AnimationStateMachine* animation_state_machine) {
						entt::entity owner_id = animation_state_machine->GetOwnerId();
						Player* player = SCENE_MGR->GetActor<Player>(owner_id);
						if (player->cur_equipped_weapon_ == EQUIPPED_WEAPON::HAND_GUN) {
							return true;
						}
						else {
							return false;
						}
					})
					});
				transitions_.insert({ AIM_POSE_GR, Transition(IDLE_POSE_MELEE,[this](const AnimationStateMachine* animation_state_machine) {
						entt::entity owner_id = animation_state_machine->GetOwnerId();
						Player* player = SCENE_MGR->GetActor<Player>(owner_id);
						if (player->cur_equipped_weapon_ == EQUIPPED_WEAPON::MELEE_WEAPON) {
							return true;
						}
						else {
							return false;
						}
					})
					});
			}

			// Attack
			{
				transitions_.insert({ AIM_POSE_MELEE, Transition(ATTACK_MELEE,[this](const AnimationStateMachine* animation_state_machine) {
						entt::entity owner_id = animation_state_machine->GetOwnerId();
						Player* player = SCENE_MGR->GetActor<Player>(owner_id);
						if (player->is_firing_ == true) {
							return true;
						}
						else {
							return false;
						}
					})
					});
				transitions_.insert({ ATTACK_MELEE, Transition(AIM_POSE_MELEE,[this](const AnimationStateMachine* animation_state_machine) {
						if (IsAnimationEnded()) {
							return true;
						}
						else {
							return false;
						}
					})
					});
			}
		}

		cur_state_ = states_[IDLE_POSE_HG];
	}

	virtual void OnUpdate() override {
		AnimationStateMachine::OnUpdate();
	}

	class IdlePoseAR : public AnimationState {
	public:
		IdlePoseAR() : AnimationState(IDLE_POSE_AR) {}
	public:
		virtual void Enter(AnimationStateMachine* animation_base) override
		{
			animation_base->SetAnimation("", 0.2f);
		}
		virtual void Exit(AnimationStateMachine* animation_base) override
		{
		}

		virtual void OnUpdate(AnimationStateMachine* animation_base) override
		{
		}
	};

	class IdlePoseHG : public AnimationState {
	public:
		IdlePoseHG() : AnimationState(IDLE_POSE_HG) {}
	public:
		virtual void Enter(AnimationStateMachine* animation_base) override
		{
		}
		virtual void Exit(AnimationStateMachine* animation_base) override
		{
		}
		virtual void OnUpdate(AnimationStateMachine* animation_base) override
		{
		}
	};

	class IdlePoseGR : public AnimationState {
	public:
		IdlePoseGR() : AnimationState(IDLE_POSE_GR) {}
	public:
		virtual void Enter(AnimationStateMachine* animation_base) override
		{
		}
		virtual void Exit(AnimationStateMachine* animation_base) override
		{
		}
		virtual void OnUpdate(AnimationStateMachine* animation_base) override
		{
		}
	};

	class AimPoseAR : public AnimationState {
	public:
		AimPoseAR() : AnimationState(AIM_POSE_AR) {}
	public:
		virtual void Enter(AnimationStateMachine* animation_base) override
		{
		}
		virtual void Exit(AnimationStateMachine* animation_base) override
		{
		}
		virtual void OnUpdate(AnimationStateMachine* animation_base) override
		{
		}
	};

	class AimPoseHG : public AnimationState {
	public:
		AimPoseHG() : AnimationState(AIM_POSE_HG) {}
	public:
		virtual void Enter(AnimationStateMachine* animation_base) override
		{
		}
		virtual void Exit(AnimationStateMachine* animation_base) override
		{
		}
		virtual void OnUpdate(AnimationStateMachine* animation_base) override
		{
		}
	};

	class AimPoseMelee : public AnimationState {
	public:
		AimPoseMelee() : AnimationState(AIM_POSE_MELEE) {}
	public:
		virtual void Enter(AnimationStateMachine* animation_base) override
		{
		}
		virtual void Exit(AnimationStateMachine* animation_base) override
		{
		}
		virtual void OnUpdate(AnimationStateMachine* animation_base) override
		{
		}
	};

	class AimPoseGR : public AnimationState {
	public:
		AimPoseGR() : AnimationState(AIM_POSE_GR) {}
	public:
		virtual void Enter(AnimationStateMachine* animation_base) override
		{
		}
		virtual void Exit(AnimationStateMachine* animation_base) override
		{
		}
		virtual void OnUpdate(AnimationStateMachine* animation_base) override
		{
		}
	};

	class AttackAR : public AnimationState {
	public:
		AttackAR() : AnimationState(ATTACK_AR) {}
	public:
		virtual void Enter(AnimationStateMachine* animation_base) override
		{
		}
		virtual void Exit(AnimationStateMachine* animation_base) override
		{
		}
		virtual void OnUpdate(AnimationStateMachine* animation_base) override
		{
		}
	};

	class AttackHG : public AnimationState {
	public:
		AttackHG() : AnimationState(ATTACK_HG) {}
	public:
		virtual void Enter(AnimationStateMachine* animation_base) override
		{
		}
		virtual void Exit(AnimationStateMachine* animation_base) override
		{
		}
		virtual void OnUpdate(AnimationStateMachine* animation_base) override
		{
		}
	};

	class AttackMelee : public AnimationState {
	public:
		AttackMelee() : AnimationState(ATTACK_MELEE) {}
	public:
		virtual void Enter(AnimationStateMachine* animation_base) override
		{
		}
		virtual void Exit(AnimationStateMachine* animation_base) override
		{
		}
		virtual void OnUpdate(AnimationStateMachine* animation_base) override
		{
		}
	};

	class AttackGR : public AnimationState {
	public:
		AttackGR() : AnimationState(ATTACK_GR) {}
	public:
		virtual void Enter(AnimationStateMachine* animation_base) override
		{
		}
		virtual void Exit(AnimationStateMachine* animation_base) override
		{
		}
		virtual void OnUpdate(AnimationStateMachine* animation_base) override
		{
		}
	};

	class Hit : public AnimationState {
	public:
		Hit() : AnimationState(HIT) {}
	public:
		virtual void Enter(AnimationStateMachine* animation_base) override
		{
		}
		virtual void Exit(AnimationStateMachine* animation_base) override
		{
		}
		virtual void OnUpdate(AnimationStateMachine* animation_base) override
		{
		}
	};

	class Die : public AnimationState {
	public:
		Die() : AnimationState(DIE) {}
	public:
		virtual void Enter(AnimationStateMachine* animation_base) override
		{
		}
		virtual void Exit(AnimationStateMachine* animation_base) override
		{
		}
		virtual void OnUpdate(AnimationStateMachine* animation_base) override
		{
		}
	};
};