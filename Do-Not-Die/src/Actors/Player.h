#pragma once
#include "Engine_include.h"
#include "AnimationStateMachine.h"

#define INVENTORY_MAX 4

class ItemBase;

using namespace reality;

class Player : public Character
{
public:
	virtual void OnInit(entt::registry& registry) override;
	virtual void OnUpdate() override;
	void SetCharacterAnimation(string anim_id, string anim_slot = "");
public:
	void MoveRight();
	void MoveRightForward();
	void MoveRightBack();
	void MoveLeft();
	void MoveLeftForward();
	void MoveLeftBack();
	void MoveForward();
	void MoveBack();
	void Jump();
	void Idle();
	void Fire();
	void Aim();

public:
	bool IsAiming();

public:
	void ResetPos();
	void SetPos(const XMVECTOR& position = { 0.f, 100.f, 0.f, 0.f });

public:
	int GetMaxHp() const;
	void SetCurHp(int hp);
	void TakeDamage(int damage);
	int GetCurHp() const;

private:
	void AddFlashLight();
	void UpdateFlashLight();

private:
	int max_hp_;
	int cur_hp_;
	bool is_aiming_ = false;
private:
	vector<shared_ptr<ItemBase>> inventory_;
public:
	bool AcquireItem(shared_ptr<ItemBase> item, int count = 1);
	vector<shared_ptr<ItemBase>>& GetInventory();
public:
	bool fire_ = false;
};

class PlayerUpperBodyAnimationStateMachine : public AnimationStateMachine {
public:
	enum States {
		BASE_POSE,
		AIM_POSE,
		FIRE,
	};

	PlayerUpperBodyAnimationStateMachine(entt::entity owner_id) : AnimationStateMachine(owner_id) {};

	virtual void OnInit() override {
		states_.insert({ BASE_POSE, make_shared<BasePose>() });
		states_.insert({ AIM_POSE, make_shared<AimPose>()});
		states_.insert({ FIRE, make_shared<Fire>()});
		transitions_.insert({ BASE_POSE, Transition(AIM_POSE,[this](const AnimationBase* animation_base) {
				Player* player = SCENE_MGR->GetActor<Player>(owner_id_);
				if (player->IsAiming()) {
					return true;
				}
				else {
					return false;
				}
			})
		});
		transitions_.insert({ AIM_POSE, Transition(BASE_POSE,[this](const AnimationBase* animation_base) {
				Player* player = SCENE_MGR->GetActor<Player>(owner_id_);
				if (player->IsAiming() == false) {
					return true;
				}
				else {
					return false;
				}
			})
		});
		transitions_.insert({ AIM_POSE, Transition(FIRE,[this](const AnimationBase* animation_base) {
				Player* player = SCENE_MGR->GetActor<Player>(owner_id_);
				if (player->fire_ == true) {
					return true;
				}
				else {
					return false;
				}
			})
		});
		transitions_.insert({ FIRE, Transition(AIM_POSE,[this](const AnimationBase* animation_base) {
				Player* player = SCENE_MGR->GetActor<Player>(owner_id_);
				if (this->IsAnimationEnded()) {
					player->fire_ = false;
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
		virtual void Enter(AnimationBase* animation_base) override
		{
			animation_base->SetAnimation("", 0.2f);
		}
		virtual void Exit(AnimationBase* animation_base) override
		{
		}

		virtual void OnUpdate(AnimationBase* animation_base) override
		{
		}
	};

	class AimPose : public AnimationState {
	public:
		AimPose() : AnimationState(AIM_POSE) {}
	public:
		virtual void Enter(AnimationBase* animation_base) override
		{
			animation_base->SetAnimation("A_TP_CH_Handgun_Aim_Pose_Retargeted_Unreal Take.anim", 0.2f);
		}
		virtual void Exit(AnimationBase* animation_base) override
		{
		}
		virtual void OnUpdate(AnimationBase* animation_base) override
		{
		}
	};

	class Fire : public AnimationState {
	public:
		Fire() : AnimationState(FIRE) {}
	public:
		virtual void Enter(AnimationBase* animation_base) override
		{
			animation_base->SetAnimation("A_TP_CH_Handgun_Fire_Retargeted_Unreal Take.anim", 0.0f);
		}
		virtual void Exit(AnimationBase* animation_base) override
		{
		}
		virtual void OnUpdate(AnimationBase* animation_base) override
		{
		}
	};
};