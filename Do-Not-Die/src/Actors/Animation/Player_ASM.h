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
		RELOAD_AR,
		RELOAD_HG,
		HIT_AR_IDLE,
		HIT_AR_AIM,
		HIT_HG_IDLE,
		HIT_HG_AIM,
		HIT_MELEE,
		HIT_GR_IDLE,
		HIT_GR_AIM,
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
		states_.insert({ RELOAD_HG, make_shared<ReloadHG>() });
		states_.insert({ RELOAD_AR, make_shared<ReloadAR>() });
		states_.insert({ HIT_AR_IDLE, make_shared<HitARIdle>() });
		states_.insert({ HIT_AR_AIM, make_shared<HitARAim>() });
		states_.insert({ HIT_HG_IDLE, make_shared<HitHGIdle>() });
		states_.insert({ HIT_HG_AIM, make_shared<HitHGAim>() });
		states_.insert({ HIT_MELEE, make_shared<HitMelee>() });
		states_.insert({ HIT_GR_IDLE, make_shared<HitGRIdle>() });
		states_.insert({ HIT_GR_AIM, make_shared<HitGRAim>() });

		// Auto Rifle
		{
			// Pose Switch
			{
				transitions_.insert({ IDLE_POSE_AR, Transition(IDLE_POSE_HG,[this](const AnimationStateMachine* animation_state_machine) {
						entt::entity owner_id = animation_state_machine->GetOwnerId();
						Player* player = SCENE_MGR->GetActor<Player>(owner_id);
						if (player->cur_equipped_weapon_ == EQUIPPED_WEAPON::HAND_GUN) {
							player->AddSoundQueue(SFX, "S_WEP_Holster.wav", 0.5f, false);
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
							player->AddSoundQueue(SFX, "S_WEP_Holster.wav", 0.5f, false);
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
							player->AddSoundQueue(SFX, "S_WEP_Holster.wav", 0.5f, false);
							return true;
						}
						else {
							return false;
						}
					})
					});
			}

			// Reload
			{
				transitions_.insert({ IDLE_POSE_AR, Transition(RELOAD_AR,[this](const AnimationStateMachine* animation_state_machine) {
						entt::entity owner_id = animation_state_machine->GetOwnerId();
						Player* player = SCENE_MGR->GetActor<Player>(owner_id);
						if (player->IsReloading() == true) {
							return true;
						}
						else {
							return false;
						}
					})
					});
				transitions_.insert({ RELOAD_AR, Transition(IDLE_POSE_AR,[this](const AnimationStateMachine* animation_state_machine) {
					entt::entity owner_id = animation_state_machine->GetOwnerId();
					Player* player = SCENE_MGR->GetActor<Player>(owner_id);
					if (IsAnimationEnded() && !player->IsAiming()) {
							return true;
						}
						else {
							return false;
						}
					})
					});
				transitions_.insert({ AIM_POSE_AR, Transition(RELOAD_AR,[this](const AnimationStateMachine* animation_state_machine) {
						entt::entity owner_id = animation_state_machine->GetOwnerId();
						Player* player = SCENE_MGR->GetActor<Player>(owner_id);
						if (player->IsReloading() == true) {
							return true;
						}
						else {
							return false;
						}
					})
					});
				transitions_.insert({ RELOAD_AR, Transition(AIM_POSE_AR,[this](const AnimationStateMachine* animation_state_machine) {
					entt::entity owner_id = animation_state_machine->GetOwnerId();
					Player* player = SCENE_MGR->GetActor<Player>(owner_id);
					if (IsAnimationEnded() && player->IsAiming()) {
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
						if (player->is_attacking_ == true) {
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

			// Hit
			{
				transitions_.insert({ IDLE_POSE_AR, Transition(HIT_AR_IDLE,[this](const AnimationStateMachine* animation_state_machine) {
						entt::entity owner_id = animation_state_machine->GetOwnerId();
						Player* player = SCENE_MGR->GetActor<Player>(owner_id);
						if (player->is_hit_ == true) {
							return true;
						}
						else {
							return false;
						}
					})
					});
				transitions_.insert({ AIM_POSE_AR, Transition(HIT_AR_AIM,[this](const AnimationStateMachine* animation_state_machine) {
						entt::entity owner_id = animation_state_machine->GetOwnerId();
						Player* player = SCENE_MGR->GetActor<Player>(owner_id);
						if (player->is_hit_ == true) {
							return true;
						}
						else {
							return false;
						}
					})
					});
				transitions_.insert({ ATTACK_AR, Transition(HIT_AR_AIM,[this](const AnimationStateMachine* animation_state_machine) {
						entt::entity owner_id = animation_state_machine->GetOwnerId();
						Player* player = SCENE_MGR->GetActor<Player>(owner_id);
						if (player->is_hit_ == true) {
							player->is_attacking_ = false;
							return true;
						}
						else {
							return false;
						}
					})
					});
				transitions_.insert({ RELOAD_AR, Transition(HIT_AR_AIM,[this](const AnimationStateMachine* animation_state_machine) {
						entt::entity owner_id = animation_state_machine->GetOwnerId();
						Player* player = SCENE_MGR->GetActor<Player>(owner_id);
						if (player->is_hit_ == true) {
							player->is_reloading_ = false;
							return true;
						}
						else {
							return false;
						}
					})
					});
				transitions_.insert({ HIT_AR_AIM, Transition(AIM_POSE_AR,[this](const AnimationStateMachine* animation_state_machine) {
						if (IsAnimationEnded()) {
							return true;
						}
						else {
							return false;
						}
					})
					});
				transitions_.insert({ HIT_AR_IDLE, Transition(IDLE_POSE_AR,[this](const AnimationStateMachine* animation_state_machine) {
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
							player->AddSoundQueue(SFX, "S_WEP_Holster.wav", 0.5f, false);
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
							player->AddSoundQueue(SFX, "S_WEP_Holster.wav", 0.5f, false);
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
							player->AddSoundQueue(SFX, "S_WEP_Holster.wav", 0.5f, false);
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

			// Reload
			{
				transitions_.insert({ IDLE_POSE_HG, Transition(RELOAD_HG,[this](const AnimationStateMachine* animation_state_machine) {
						entt::entity owner_id = animation_state_machine->GetOwnerId();
						Player* player = SCENE_MGR->GetActor<Player>(owner_id);
						if (player->IsReloading() == true) {
							return true;
						}
						else {
							return false;
						}
					})
					});
				transitions_.insert({ RELOAD_HG, Transition(IDLE_POSE_HG,[this](const AnimationStateMachine* animation_state_machine) {
						entt::entity owner_id = animation_state_machine->GetOwnerId();
						Player* player = SCENE_MGR->GetActor<Player>(owner_id);
						if (IsAnimationEnded() && !player->IsAiming()) {
							return true;
						}
						else {
							return false;
						}
					})
					});
				transitions_.insert({ AIM_POSE_HG, Transition(RELOAD_HG,[this](const AnimationStateMachine* animation_state_machine) {
						entt::entity owner_id = animation_state_machine->GetOwnerId();
						Player* player = SCENE_MGR->GetActor<Player>(owner_id);
						if (player->IsReloading() == true) {
							return true;
						}
						else {
							return false;
						}
					})
					});
				transitions_.insert({ RELOAD_HG, Transition(AIM_POSE_HG,[this](const AnimationStateMachine* animation_state_machine) {
						entt::entity owner_id = animation_state_machine->GetOwnerId();
						Player* player = SCENE_MGR->GetActor<Player>(owner_id);
						if (IsAnimationEnded() && player->IsAiming()) {
							return true;
						}
						else {
							return false;
						}
					})
					});
				transitions_.insert({ RELOAD_HG, Transition(IDLE_POSE_HG,[this](const AnimationStateMachine* animation_state_machine) {
						entt::entity owner_id = animation_state_machine->GetOwnerId();
						Player* player = SCENE_MGR->GetActor<Player>(owner_id);
						if (IsAnimationEnded() && !player->IsAiming()) {
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
						if (player->is_attacking_ == true) {
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

			// Hit
			{
				transitions_.insert({ IDLE_POSE_HG, Transition(HIT_HG_IDLE,[this](const AnimationStateMachine* animation_state_machine) {
						entt::entity owner_id = animation_state_machine->GetOwnerId();
						Player* player = SCENE_MGR->GetActor<Player>(owner_id);
						if (player->is_hit_ == true) {
							return true;
						}
						else {
							return false;
						}
					})
					});
				transitions_.insert({ AIM_POSE_HG, Transition(HIT_HG_AIM,[this](const AnimationStateMachine* animation_state_machine) {
						entt::entity owner_id = animation_state_machine->GetOwnerId();
						Player* player = SCENE_MGR->GetActor<Player>(owner_id);
						if (player->is_hit_ == true) {
							return true;
						}
						else {
							return false;
						}
					})
					});
				transitions_.insert({ ATTACK_HG, Transition(HIT_HG_AIM,[this](const AnimationStateMachine* animation_state_machine) {
						entt::entity owner_id = animation_state_machine->GetOwnerId();
						Player* player = SCENE_MGR->GetActor<Player>(owner_id);
						if (player->is_hit_ == true) {
							player->is_attacking_ = false;
							return true;
						}
						else {
							return false;
						}
					})
					});
				transitions_.insert({ RELOAD_HG, Transition(HIT_HG_AIM,[this](const AnimationStateMachine* animation_state_machine) {
						entt::entity owner_id = animation_state_machine->GetOwnerId();
						Player* player = SCENE_MGR->GetActor<Player>(owner_id);
						if (player->is_hit_ == true) {
							player->is_reloading_ = false;
							return true;
						}
						else {
							return false;
						}
					})
					});
				transitions_.insert({ HIT_HG_AIM, Transition(AIM_POSE_HG,[this](const AnimationStateMachine* animation_state_machine) {
						if (IsAnimationEnded()) {
							return true;
						}
						else {
							return false;
						}
					})
					});
				transitions_.insert({ HIT_HG_IDLE, Transition(IDLE_POSE_HG,[this](const AnimationStateMachine* animation_state_machine) {
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
							player->AddSoundQueue(SFX, "S_WEP_Holster.wav", 0.5f, false);
							return true;
						}
						else {
							return false;
						}
					})
					});
				transitions_.insert({ AIM_POSE_MELEE, Transition(IDLE_POSE_HG,[this](const AnimationStateMachine* animation_state_machine) {
						entt::entity owner_id = animation_state_machine->GetOwnerId();
						Player* player = SCENE_MGR->GetActor<Player>(owner_id);
						if (player->cur_equipped_weapon_ == EQUIPPED_WEAPON::HAND_GUN) {
							player->AddSoundQueue(SFX, "S_WEP_Holster.wav", 0.5f, false);
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
							player->AddSoundQueue(SFX, "S_WEP_Holster.wav", 0.5f, false);
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
						if (player->is_attacking_ == true) {
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

			// Hit
			{
				transitions_.insert({ AIM_POSE_MELEE, Transition(HIT_MELEE,[this](const AnimationStateMachine* animation_state_machine) {
						entt::entity owner_id = animation_state_machine->GetOwnerId();
						Player* player = SCENE_MGR->GetActor<Player>(owner_id);
						if (player->is_hit_ == true) {
							return true;
						}
						else {
							return false;
						}
					})
					});
				transitions_.insert({ ATTACK_MELEE, Transition(HIT_MELEE,[this](const AnimationStateMachine* animation_state_machine) {
						entt::entity owner_id = animation_state_machine->GetOwnerId();
						Player* player = SCENE_MGR->GetActor<Player>(owner_id);
						if (player->is_hit_ == true) {
							player->is_attacking_ = false;
							return true;
						}
						else {
							return false;
						}
					})
					});
				transitions_.insert({ HIT_MELEE, Transition(AIM_POSE_MELEE,[this](const AnimationStateMachine* animation_state_machine) {
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
				transitions_.insert({ IDLE_POSE_GR, Transition(IDLE_POSE_AR,[this](const AnimationStateMachine* animation_state_machine) {
						entt::entity owner_id = animation_state_machine->GetOwnerId();
						Player* player = SCENE_MGR->GetActor<Player>(owner_id);
						if (player->cur_equipped_weapon_ == EQUIPPED_WEAPON::AUTO_RIFLE) {
							player->AddSoundQueue(SFX, "S_WEP_Holster.wav", 0.5f, false);
							return true;
						}
						else {
							return false;
						}
					})
					});
				transitions_.insert({ IDLE_POSE_GR, Transition(IDLE_POSE_HG,[this](const AnimationStateMachine* animation_state_machine) {
						entt::entity owner_id = animation_state_machine->GetOwnerId();
						Player* player = SCENE_MGR->GetActor<Player>(owner_id);
						if (player->cur_equipped_weapon_ == EQUIPPED_WEAPON::HAND_GUN) {
							player->AddSoundQueue(SFX, "S_WEP_Holster.wav", 0.5f, false);
							return true;
						}
						else {
							return false;
						}
					})
					});
				transitions_.insert({ IDLE_POSE_GR, Transition(AIM_POSE_MELEE,[this](const AnimationStateMachine* animation_state_machine) {
						entt::entity owner_id = animation_state_machine->GetOwnerId();
						Player* player = SCENE_MGR->GetActor<Player>(owner_id);
						if (player->cur_equipped_weapon_ == EQUIPPED_WEAPON::MELEE_WEAPON) {
							player->AddSoundQueue(SFX, "S_WEP_Holster.wav", 0.5f, false);
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
				transitions_.insert({ AIM_POSE_GR, Transition(ATTACK_GR,[this](const AnimationStateMachine* animation_state_machine) {
						entt::entity owner_id = animation_state_machine->GetOwnerId();
						Player* player = SCENE_MGR->GetActor<Player>(owner_id);
						if (player->is_attacking_ == true) {
							return true;
						}
						else {
							return false;
						}
					})
					});
				transitions_.insert({ ATTACK_GR, Transition(IDLE_POSE_GR,[this](const AnimationStateMachine* animation_state_machine) {
						if (IsAnimationEnded()) {
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
				transitions_.insert({ IDLE_POSE_GR, Transition(AIM_POSE_GR,[this](const AnimationStateMachine* animation_state_machine) {
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
				transitions_.insert({ AIM_POSE_GR, Transition(IDLE_POSE_GR,[this](const AnimationStateMachine* animation_state_machine) {
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

			// Hit
			{
				transitions_.insert({ AIM_POSE_GR, Transition(HIT_GR_AIM,[this](const AnimationStateMachine* animation_state_machine) {
						entt::entity owner_id = animation_state_machine->GetOwnerId();
						Player* player = SCENE_MGR->GetActor<Player>(owner_id);
						if (player->is_hit_ == true) {
							return true;
						}
						else {
							return false;
						}
					})
					});
				transitions_.insert({ IDLE_POSE_GR, Transition(HIT_GR_IDLE,[this](const AnimationStateMachine* animation_state_machine) {
						entt::entity owner_id = animation_state_machine->GetOwnerId();
						Player* player = SCENE_MGR->GetActor<Player>(owner_id);
						if (player->is_hit_ == true) {
							return true;
						}
						else {
							return false;
						}
					})
					});
				transitions_.insert({ ATTACK_GR, Transition(HIT_GR_AIM,[this](const AnimationStateMachine* animation_state_machine) {
						entt::entity owner_id = animation_state_machine->GetOwnerId();
						Player* player = SCENE_MGR->GetActor<Player>(owner_id);
						if (player->is_hit_ == true) {
							player->is_attacking_ = false;
							return true;
						}
						else {
							return false;
						}
					})
					});
				transitions_.insert({ HIT_GR_AIM, Transition(AIM_POSE_GR,[this](const AnimationStateMachine* animation_state_machine) {
						if (IsAnimationEnded()) {
							return true;
						}
						else {
							return false;
						}
					})
					});
				transitions_.insert({ HIT_GR_IDLE, Transition(IDLE_POSE_GR,[this](const AnimationStateMachine* animation_state_machine) {
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

	private:
		Player* player_;
	public:
		Player* GetPlayer() { return player_; }
		void SetPlayer(Player* player) { player_ = player; }

	public:
	class IdlePoseAR : public AnimationState {
	public:
		IdlePoseAR() : AnimationState(IDLE_POSE_AR) {}
	public:
		virtual void Enter(AnimationStateMachine* animation_base) override
		{
			animation_base->SetAnimation("player_ar_idle_pose.anim", 0.3f);
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
			animation_base->SetAnimation("", 0.3f);
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
			animation_base->SetAnimation("", 0.3f);
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
			animation_base->SetAnimation("player_ar_aim_pose.anim", 0.3f);
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
			animation_base->SetAnimation("player_hg_aim_pose.anim", 0.3f);
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
			animation_base->SetAnimation("", 0.3f);
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
			animation_base->SetAnimation("player_gr_aim_pose.anim", 0.3f);
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
			animation_base->SetAnimation("player_ar_fire.anim", 0.0f);
		}
		virtual void Exit(AnimationStateMachine* animation_base) override
		{
			Player* player = SCENE_MGR->GetActor<Player>(animation_base->GetOwnerId());
			player->is_attacking_ = false;
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
			animation_base->SetAnimation("player_hg_fire.anim", 0.0f);
		}
		virtual void Exit(AnimationStateMachine* animation_base) override
		{
			Player* player = SCENE_MGR->GetActor<Player>(animation_base->GetOwnerId());
			player->is_attacking_ = false;
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
			animation_base->SetAnimation("player_melee_attack.anim", 0.1f);
		}
		virtual void Exit(AnimationStateMachine* animation_base) override
		{
			Player* player = SCENE_MGR->GetActor<Player>(animation_base->GetOwnerId());
			player->is_attacking_ = false;
		}
		virtual void OnUpdate(AnimationStateMachine* animation_base) override
		{
		}
	};

	class AttackGR : public AnimationState {
	public:
		AttackGR() : AnimationState(ATTACK_GR) {}
	public:
		bool executed;
		virtual void Enter(AnimationStateMachine* animation_base) override
		{
			executed = false;
			animation_base->SetAnimation("player_gr_throw.anim", 0.2f);
		}
		virtual void Exit(AnimationStateMachine* animation_base) override
		{
			auto entity = animation_base->GetOwnerId();
			Player* player = SCENE_MGR->GetActor<Player>(entity);
			
			player->is_attacking_ = false;
			if (player->cur_weapon_using_remained_[(int)EQUIPPED_WEAPON::GRENADE] > 0)
			{
				auto static_mesh = player->reg_scene_->try_get<C_StaticMesh>(entity);
				static_mesh->static_mesh_id = "Grenade.stmesh";
			}
			
		}
		virtual void OnUpdate(AnimationStateMachine* animation_base) override
		{
			if (animation_base->GetCurAnimation().cur_frame_ > 36.0f && !executed)
			{
				Player* player = SCENE_MGR->GetActor<Player>(animation_base->GetOwnerId());
				auto entity = animation_base->GetOwnerId();
				player->ThrowGrenade();
				auto static_mesh = player->reg_scene_->try_get<C_StaticMesh>(entity);
				static_mesh->static_mesh_id = "";
				executed = true;
			}
		}
	};

	class ReloadAR : public AnimationState {
	public:
		ReloadAR() : AnimationState(RELOAD_AR) {}
	public:
		virtual void Enter(AnimationStateMachine* animation_base) override
		{
			animation_base->SetAnimation("player_ar_reload.anim", 0.3f);
			auto sm = (PlayerUpperBodyAnimationStateMachine*)animation_base;
			sm->GetPlayer()->AddSoundQueue(SFX, "S_WEP_AR_01_Reload.wav", 0.5f, false);
		}
		virtual void Exit(AnimationStateMachine* animation_base) override
		{
			Player* player = SCENE_MGR->GetActor<Player>(animation_base->GetOwnerId());
			player->is_reloading_ = false;

			int count_to_reload = AUTO_RIFLE_MAX - player->cur_weapon_using_remained_[(int)EQUIPPED_WEAPON::AUTO_RIFLE];
			if (count_to_reload >= player->cur_weapon_total_remained_[(int)EQUIPPED_WEAPON::AUTO_RIFLE])
			{
				player->cur_weapon_using_remained_[(int)EQUIPPED_WEAPON::AUTO_RIFLE] += player->cur_weapon_total_remained_[(int)EQUIPPED_WEAPON::AUTO_RIFLE];
				player->cur_weapon_total_remained_[(int)EQUIPPED_WEAPON::AUTO_RIFLE] = 0;
			}
			else
			{
				player->cur_weapon_using_remained_[(int)EQUIPPED_WEAPON::AUTO_RIFLE] += count_to_reload;
				player->cur_weapon_total_remained_[(int)EQUIPPED_WEAPON::AUTO_RIFLE] -= count_to_reload;
			}
			
		}
		virtual void OnUpdate(AnimationStateMachine* animation_base) override
		{
		}
	};

	class ReloadHG : public AnimationState {
	public:
		ReloadHG() : AnimationState(RELOAD_HG) {}
	public:
		virtual void Enter(AnimationStateMachine* animation_base) override
		{
			animation_base->SetAnimation("player_hg_reload.anim", 0.3f);
			auto sm = (PlayerUpperBodyAnimationStateMachine*)animation_base;
			sm->GetPlayer()->AddSoundQueue(SFX, "S_WEP_Handgun_Reload.wav", 0.5f, false);
		}
		virtual void Exit(AnimationStateMachine* animation_base) override
		{
			Player* player = SCENE_MGR->GetActor<Player>(animation_base->GetOwnerId());
			player->is_reloading_ = false;

			int count_to_reload = HAND_GUN_MAX - player->cur_weapon_using_remained_[(int)EQUIPPED_WEAPON::HAND_GUN];
			if (count_to_reload >= player->cur_weapon_total_remained_[(int)EQUIPPED_WEAPON::HAND_GUN])
			{
				player->cur_weapon_using_remained_[(int)EQUIPPED_WEAPON::HAND_GUN] += player->cur_weapon_total_remained_[(int)EQUIPPED_WEAPON::HAND_GUN];
				player->cur_weapon_total_remained_[(int)EQUIPPED_WEAPON::HAND_GUN] = 0;
			}
			else
			{
				player->cur_weapon_using_remained_[(int)EQUIPPED_WEAPON::HAND_GUN] += count_to_reload;
				player->cur_weapon_total_remained_[(int)EQUIPPED_WEAPON::HAND_GUN] -= count_to_reload;
			}
		}
		virtual void OnUpdate(AnimationStateMachine* animation_base) override
		{
		}
	};

	class HitARIdle : public AnimationState {
	public:
		HitARIdle() : AnimationState(HIT_AR_IDLE) {}
	public:
		virtual void Enter(AnimationStateMachine* animation_base) override
		{
			animation_base->SetAnimation("player_ar_idle_pose_hit_react.anim", 0.0f);
		}
		virtual void Exit(AnimationStateMachine* animation_base) override
		{
			entt::entity owner_id = animation_base->GetOwnerId();
			Player* player = SCENE_MGR->GetActor<Player>(owner_id);
			player->is_hit_ = false;
		}
		virtual void OnUpdate(AnimationStateMachine* animation_base) override
		{
		}
	};

	class HitARAim : public AnimationState {
	public:
		HitARAim() : AnimationState(HIT_AR_AIM) {}
	public:
		virtual void Enter(AnimationStateMachine* animation_base) override
		{
			animation_base->SetAnimation("player_ar_aim_pose_hit_react.anim", 0.0f);
		}
		virtual void Exit(AnimationStateMachine* animation_base) override
		{
			entt::entity owner_id = animation_base->GetOwnerId();
			Player* player = SCENE_MGR->GetActor<Player>(owner_id);
			player->is_hit_ = false;
		}
		virtual void OnUpdate(AnimationStateMachine* animation_base) override
		{
		}
	};

	class HitHGIdle : public AnimationState {
	public:
		HitHGIdle() : AnimationState(HIT_HG_IDLE) {}
	public:
		virtual void Enter(AnimationStateMachine* animation_base) override
		{
			animation_base->SetAnimation("player_idle_hit_react.anim", 0.5f);
		}
		virtual void Exit(AnimationStateMachine* animation_base) override
		{
			entt::entity owner_id = animation_base->GetOwnerId();
			Player* player = SCENE_MGR->GetActor<Player>(owner_id);
			player->is_hit_ = false;
		}
		virtual void OnUpdate(AnimationStateMachine* animation_base) override
		{
		}
	};

	class HitHGAim : public AnimationState {
	public:
		HitHGAim() : AnimationState(HIT_HG_AIM) {}
	public:
		virtual void Enter(AnimationStateMachine* animation_base) override
		{
			animation_base->SetAnimation("player_hg_aim_pose_hit_react.anim", 0.0f);
		}
		virtual void Exit(AnimationStateMachine* animation_base) override
		{
			entt::entity owner_id = animation_base->GetOwnerId();
			Player* player = SCENE_MGR->GetActor<Player>(owner_id);
			player->is_hit_ = false;
		}
		virtual void OnUpdate(AnimationStateMachine* animation_base) override
		{
		}
	};

	class HitMelee : public AnimationState {
	public:
		HitMelee() : AnimationState(HIT_MELEE) {}
	public:
		virtual void Enter(AnimationStateMachine* animation_base) override
		{

			animation_base->SetAnimation("player_idle_hit_react.anim", 0.0f);
		}
		virtual void Exit(AnimationStateMachine* animation_base) override
		{
			entt::entity owner_id = animation_base->GetOwnerId();
			Player* player = SCENE_MGR->GetActor<Player>(owner_id);
			player->is_hit_ = false;
		}
		virtual void OnUpdate(AnimationStateMachine* animation_base) override
		{
		}
	};

	class HitGRIdle : public AnimationState {
	public:
		HitGRIdle() : AnimationState(HIT_GR_IDLE) {}
	public:
		virtual void Enter(AnimationStateMachine* animation_base) override
		{
			animation_base->SetAnimation("player_idle_hit_react.anim", 0.0f);
		}
		virtual void Exit(AnimationStateMachine* animation_base) override
		{
			entt::entity owner_id = animation_base->GetOwnerId();
			Player* player = SCENE_MGR->GetActor<Player>(owner_id);
			player->is_hit_ = false;
		}
		virtual void OnUpdate(AnimationStateMachine* animation_base) override
		{
		}
	};

	class HitGRAim : public AnimationState {
	public:
		HitGRAim() : AnimationState(HIT_GR_AIM) {}
	public:
		virtual void Enter(AnimationStateMachine* animation_base) override
		{
			animation_base->SetAnimation("player_gr_aim_pose_hit_react.anim", 0.0f);
		}
		virtual void Exit(AnimationStateMachine* animation_base) override
		{
			entt::entity owner_id = animation_base->GetOwnerId();
			Player* player = SCENE_MGR->GetActor<Player>(owner_id);
			player->is_hit_ = false;
		}
		virtual void OnUpdate(AnimationStateMachine* animation_base) override
		{
		}
	};
};