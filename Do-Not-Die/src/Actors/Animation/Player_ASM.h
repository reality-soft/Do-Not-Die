#pragma once
#include "Engine_include.h"
#include "AnimationStateMachine.h"
#include "AnimationState.h"
#include "Player.h"
using namespace reality;

class PlayerUpperBodyAnimationStateMachine : public AnimationStateMachine {
public:
	enum States {
		BASE_POSE,
		AIM_POSE,
		FIRE,
	};

	PlayerUpperBodyAnimationStateMachine(entt::entity owner_id, string skeletal_mesh_id, int range, string bone_name = "") : AnimationStateMachine(owner_id, skeletal_mesh_id, range, bone_name) {};

	virtual void OnInit() override {
		states_.insert({ BASE_POSE, make_shared<BasePose>() });
		states_.insert({ AIM_POSE, make_shared<AimPose>() });
		states_.insert({ FIRE, make_shared<Fire>() });
		transitions_.insert({ BASE_POSE, Transition(AIM_POSE,[this](const AnimationStateMachine* animation_base) {
				Player* player = SCENE_MGR->GetActor<Player>(owner_id_);
				if (player->IsAiming()) {
					return true;
				}
				else {
					return false;
				}
			})
			});
		transitions_.insert({ AIM_POSE, Transition(BASE_POSE,[this](const AnimationStateMachine* animation_base) {
				Player* player = SCENE_MGR->GetActor<Player>(owner_id_);
				if (player->IsAiming() == false) {
					return true;
				}
				else {
					return false;
				}
			})
			});
		transitions_.insert({ AIM_POSE, Transition(FIRE,[this](const AnimationStateMachine* animation_base) {
				Player* player = SCENE_MGR->GetActor<Player>(owner_id_);
				if (player->is_firing_ == true) {
					return true;
				}
				else {
					return false;
				}
			})
			});
		transitions_.insert({ FIRE, Transition(AIM_POSE,[this](const AnimationStateMachine* animation_base) {
				Player* player = SCENE_MGR->GetActor<Player>(owner_id_);
				if (this->IsAnimationEnded()) {
					return true;
				}
				else {
					return false;
				}
			})
			});

		cur_state_ = states_[BASE_POSE];
	}

	virtual void OnUpdate() override {
		AnimationStateMachine::OnUpdate();
	}

	class BasePose : public AnimationState {
	public:
		BasePose() : AnimationState(BASE_POSE) {}
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

	class AimPose : public AnimationState {
	public:
		AimPose() : AnimationState(AIM_POSE) {}
	public:
		virtual void Enter(AnimationStateMachine* animation_base) override
		{
			animation_base->SetAnimation("A_TP_CH_Handgun_Aim_Pose_Retargeted_Unreal Take.anim", 0.2f);
		}
		virtual void Exit(AnimationStateMachine* animation_base) override
		{
		}
		virtual void OnUpdate(AnimationStateMachine* animation_base) override
		{
		}
	};

	class Fire : public AnimationState {
	public:
		Fire() : AnimationState(FIRE) {}
	public:
		virtual void Enter(AnimationStateMachine* animation_base) override
		{
			animation_base->SetAnimation("A_TP_CH_Handgun_Fire_Retargeted_Unreal Take.anim", 0.0f);
		}
		virtual void Exit(AnimationStateMachine* animation_base) override
		{
			Player* player = SCENE_MGR->GetActor<Player>(animation_base->GetOwnerId());
			player->is_firing_ = false;
		}
		virtual void OnUpdate(AnimationStateMachine* animation_base) override
		{
		}
	};
};