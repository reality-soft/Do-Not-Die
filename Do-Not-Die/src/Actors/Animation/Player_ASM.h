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
							EVENT->PushEvent<SoundGenerateEvent>(player->entity_id_, SFX, "HalfLife_SwitchWeapon.wav", 0.5f, false);
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
							EVENT->PushEvent<SoundGenerateEvent>(player->entity_id_, SFX, "HalfLife_SwitchWeapon.wav", 0.5f, false);
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
							EVENT->PushEvent<SoundGenerateEvent>(player->entity_id_, SFX, "HalfLife_SwitchWeapon.wav", 0.5f, false);
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
							EVENT->PushEvent<SoundGenerateEvent>(player->entity_id_, SFX, "HalfLife_SwitchWeapon.wav", 0.5f, false);
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
							EVENT->PushEvent<SoundGenerateEvent>(player->entity_id_, SFX, "HalfLife_SwitchWeapon.wav", 0.5f, false);
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
							EVENT->PushEvent<SoundGenerateEvent>(player->entity_id_, SFX, "HalfLife_SwitchWeapon.wav", 0.5f, false);
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
							EVENT->PushEvent<SoundGenerateEvent>(player->entity_id_, SFX, "HalfLife_SwitchWeapon.wav", 0.5f, false);
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
							EVENT->PushEvent<SoundGenerateEvent>(player->entity_id_, SFX, "HalfLife_SwitchWeapon.wav", 0.5f, false);
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
							EVENT->PushEvent<SoundGenerateEvent>(player->entity_id_, SFX, "HalfLife_SwitchWeapon.wav", 0.5f, false);
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
							EVENT->PushEvent<SoundGenerateEvent>(player->entity_id_, SFX, "HalfLife_SwitchWeapon.wav", 0.5f, false);
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
							EVENT->PushEvent<SoundGenerateEvent>(player->entity_id_, SFX, "HalfLife_SwitchWeapon.wav", 0.5f, false);
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
							EVENT->PushEvent<SoundGenerateEvent>(player->entity_id_, SFX, "HalfLife_SwitchWeapon.wav", 0.5f, false);
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
		virtual void Enter(AnimationStateMachine* animation_state_machine) override
		{
			animation_state_machine->SetAnimation("player_ar_idle_pose.anim", 0.3f, true);
		}
		virtual void Exit(AnimationStateMachine* animation_state_machine) override
		{
		}
		virtual void OnUpdate(AnimationStateMachine* animation_state_machine) override
		{
		}
	};

	class IdlePoseHG : public AnimationState {
	public:
		IdlePoseHG() : AnimationState(IDLE_POSE_HG) {}
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

	class IdlePoseGR : public AnimationState {
	public:
		IdlePoseGR() : AnimationState(IDLE_POSE_GR) {}
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

	class AimPoseAR : public AnimationState {
	public:
		AimPoseAR() : AnimationState(AIM_POSE_AR) {}
	public:
		virtual void Enter(AnimationStateMachine* animation_state_machine) override
		{
			animation_state_machine->SetAnimation("player_ar_aim_pose.anim", 0.3f, true);
			auto sm = (PlayerUpperBodyAnimationStateMachine*)animation_state_machine;
			EVENT->PushEvent<SoundGenerateEvent>(sm->GetPlayer()->entity_id_, SFX, "S_WEP_Aim_In.wav", 1.0f, false);
		}
		virtual void Exit(AnimationStateMachine* animation_state_machine) override
		{
		}
		virtual void OnUpdate(AnimationStateMachine* animation_state_machine) override
		{
		}
	};

	class AimPoseHG : public AnimationState {
	public:
		AimPoseHG() : AnimationState(AIM_POSE_HG) {}
	public:
		virtual void Enter(AnimationStateMachine* animation_state_machine) override
		{
			animation_state_machine->SetAnimation("player_hg_aim_pose.anim", 0.3f, true);
			auto sm = (PlayerUpperBodyAnimationStateMachine*)animation_state_machine;
			EVENT->PushEvent<SoundGenerateEvent>(sm->GetPlayer()->entity_id_, SFX, "S_WEP_Aim_In.wav", 1.0f, false);
		}
		virtual void Exit(AnimationStateMachine* animation_state_machine) override
		{
		}
		virtual void OnUpdate(AnimationStateMachine* animation_state_machine) override
		{
		}
	};

	class AimPoseMelee : public AnimationState {
	public:
		AimPoseMelee() : AnimationState(AIM_POSE_MELEE) {}
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

	class AimPoseGR : public AnimationState {
	public:
		AimPoseGR() : AnimationState(AIM_POSE_GR) {}
	public:
		virtual void Enter(AnimationStateMachine* animation_state_machine) override
		{
			animation_state_machine->SetAnimation("player_gr_aim_pose.anim", 0.3f, true);
		}
		virtual void Exit(AnimationStateMachine* animation_state_machine) override
		{
		}
		virtual void OnUpdate(AnimationStateMachine* animation_state_machine) override
		{
		}
	};

	class AttackAR : public AnimationState {
	public:
		AttackAR() : AnimationState(ATTACK_AR) {}
	public:
		virtual void Enter(AnimationStateMachine* animation_state_machine) override
		{
			animation_state_machine->SetAnimation("player_ar_fire.anim", 0.0f, true);
		}
		virtual void Exit(AnimationStateMachine* animation_state_machine) override
		{
			Player* player = SCENE_MGR->GetActor<Player>(animation_state_machine->GetOwnerId());
			player->is_attacking_ = false;
		}
		virtual void OnUpdate(AnimationStateMachine* animation_state_machine) override
		{
		}
	};

	class AttackHG : public AnimationState {
	public:
		AttackHG() : AnimationState(ATTACK_HG) {}
	public:
		virtual void Enter(AnimationStateMachine* animation_state_machine) override
		{
			animation_state_machine->SetAnimation("player_hg_fire.anim", 0.0f, true);
		}
		virtual void Exit(AnimationStateMachine* animation_state_machine) override
		{
			Player* player = SCENE_MGR->GetActor<Player>(animation_state_machine->GetOwnerId());
			player->is_attacking_ = false;
		}
		virtual void OnUpdate(AnimationStateMachine* animation_state_machine) override
		{
		}
	};

	class AttackMelee : public AnimationState {
	public:
		AttackMelee() : AnimationState(ATTACK_MELEE) {}
	public:
		bool executed;
		virtual void Enter(AnimationStateMachine* animation_state_machine) override
		{
			executed = false;
			animation_state_machine->SetAnimation("player_melee_attack.anim", 0.1f, true);

			Player* player = SCENE_MGR->GetActor<Player>(animation_state_machine->GetOwnerId());
			EVENT->PushEvent<SoundGenerateEvent>(player->entity_id_, SFX, "S_WEP_Axe_Swing.wav", 1.0f, false);
		}
		virtual void Exit(AnimationStateMachine* animation_state_machine) override
		{
			Player* player = SCENE_MGR->GetActor<Player>(animation_state_machine->GetOwnerId());
			player->is_attacking_ = false;
		}
		virtual void OnUpdate(AnimationStateMachine* animation_state_machine) override
		{
			if (animation_state_machine->GetCurAnimation().cur_frame_ > 24.0f && !executed)
			{
				Player* player = SCENE_MGR->GetActor<Player>(animation_state_machine->GetOwnerId());
				if (player)
				{
					player->MeeleAttack();
					executed = true;
				}				
			}
		}
	};

	class AttackGR : public AnimationState {
	public:
		AttackGR() : AnimationState(ATTACK_GR) {}
	public:
		bool executed;
		virtual void Enter(AnimationStateMachine* animation_state_machine) override
		{
			executed = false;
			animation_state_machine->SetAnimation("player_gr_throw.anim", 0.2f, true);
			auto sm = (PlayerUpperBodyAnimationStateMachine*)animation_state_machine;
			EVENT->PushEvent<SoundGenerateEvent>(sm->GetPlayer()->entity_id_, SFX, "S_CH_Grenade_Throw.wav", 1.0f, false);
		}
		virtual void Exit(AnimationStateMachine* animation_state_machine) override
		{
			auto entity = animation_state_machine->GetOwnerId();
			Player* player = SCENE_MGR->GetActor<Player>(entity);
			
			player->is_attacking_ = false;
			if (player->cur_weapon_using_remained_[(int)EQUIPPED_WEAPON::GRENADE] > 0)
			{
				auto static_mesh = player->reg_scene_->try_get<C_StaticMesh>(entity);
				static_mesh->static_mesh_id = "Grenade.stmesh";
			}
			
		}
		virtual void OnUpdate(AnimationStateMachine* animation_state_machine) override
		{
			if (animation_state_machine->GetCurAnimation().cur_frame_ > 30.0f && !executed)
			{
				auto sm = (PlayerUpperBodyAnimationStateMachine*)animation_state_machine;
				auto entity = animation_state_machine->GetOwnerId();
				sm->GetPlayer()->ThrowGrenade();
				auto static_mesh = sm->GetPlayer()->reg_scene_->try_get<C_StaticMesh>(entity);
				static_mesh->static_mesh_id = "";
				executed = true;
			}
		}
	};

	class ReloadAR : public AnimationState {
	public:
		ReloadAR() : AnimationState(RELOAD_AR) {}
	public:
		virtual void Enter(AnimationStateMachine* animation_state_machine) override
		{
			animation_state_machine->SetAnimation("player_ar_reload.anim", 0.3f, true);
			auto sm = (PlayerUpperBodyAnimationStateMachine*)animation_state_machine;
			EVENT->PushEvent<SoundGenerateEvent>(sm->GetPlayer()->entity_id_, SFX, "S_WEP_AR_01_Reload.wav", 1.0f, false);
		}
		virtual void Exit(AnimationStateMachine* animation_state_machine) override
		{
			Player* player = SCENE_MGR->GetActor<Player>(animation_state_machine->GetOwnerId());
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
		virtual void OnUpdate(AnimationStateMachine* animation_state_machine) override
		{
		}
	};

	class ReloadHG : public AnimationState {
	public:
		ReloadHG() : AnimationState(RELOAD_HG) {}
	public:
		virtual void Enter(AnimationStateMachine* animation_state_machine) override
		{
			animation_state_machine->SetAnimation("player_hg_reload.anim", 0.3f, true);
			auto sm = (PlayerUpperBodyAnimationStateMachine*)animation_state_machine;
			EVENT->PushEvent<SoundGenerateEvent>(sm->GetPlayer()->entity_id_, SFX, "S_WEP_Handgun_Reload.wav", 1.0f, false);
		}
		virtual void Exit(AnimationStateMachine* animation_state_machine) override
		{
			Player* player = SCENE_MGR->GetActor<Player>(animation_state_machine->GetOwnerId());
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
		virtual void OnUpdate(AnimationStateMachine* animation_state_machine) override
		{
		}
	};

	class HitARIdle : public AnimationState {
	public:
		HitARIdle() : AnimationState(HIT_AR_IDLE) {}
	public:
		virtual void Enter(AnimationStateMachine* animation_state_machine) override
		{
			animation_state_machine->SetAnimation("player_ar_idle_pose_hit_react.anim", 0.0f, true);
		}
		virtual void Exit(AnimationStateMachine* animation_state_machine) override
		{
			entt::entity owner_id = animation_state_machine->GetOwnerId();
			Player* player = SCENE_MGR->GetActor<Player>(owner_id);
			player->is_hit_ = false;
		}
		virtual void OnUpdate(AnimationStateMachine* animation_state_machine) override
		{
		}
	};

	class HitARAim : public AnimationState {
	public:
		HitARAim() : AnimationState(HIT_AR_AIM) {}
	public:
		virtual void Enter(AnimationStateMachine* animation_state_machine) override
		{
			animation_state_machine->SetAnimation("player_ar_aim_pose_hit_react.anim", 0.0f, true);
		}
		virtual void Exit(AnimationStateMachine* animation_state_machine) override
		{
			entt::entity owner_id = animation_state_machine->GetOwnerId();
			Player* player = SCENE_MGR->GetActor<Player>(owner_id);
			player->is_hit_ = false;
		}
		virtual void OnUpdate(AnimationStateMachine* animation_state_machine) override
		{
		}
	};

	class HitHGIdle : public AnimationState {
	public:
		HitHGIdle() : AnimationState(HIT_HG_IDLE) {}
	public:
		virtual void Enter(AnimationStateMachine* animation_state_machine) override
		{
			animation_state_machine->SetAnimation("player_idle_hit_react.anim", 0.5f, true);
		}
		virtual void Exit(AnimationStateMachine* animation_state_machine) override
		{
			entt::entity owner_id = animation_state_machine->GetOwnerId();
			Player* player = SCENE_MGR->GetActor<Player>(owner_id);
			player->is_hit_ = false;
		}
		virtual void OnUpdate(AnimationStateMachine* animation_state_machine) override
		{
		}
	};

	class HitHGAim : public AnimationState {
	public:
		HitHGAim() : AnimationState(HIT_HG_AIM) {}
	public:
		virtual void Enter(AnimationStateMachine* animation_state_machine) override
		{
			animation_state_machine->SetAnimation("player_hg_aim_pose_hit_react.anim", 0.0f, true);
		}
		virtual void Exit(AnimationStateMachine* animation_state_machine) override
		{
			entt::entity owner_id = animation_state_machine->GetOwnerId();
			Player* player = SCENE_MGR->GetActor<Player>(owner_id);
			player->is_hit_ = false;
		}
		virtual void OnUpdate(AnimationStateMachine* animation_state_machine) override
		{
		}
	};

	class HitMelee : public AnimationState {
	public:
		HitMelee() : AnimationState(HIT_MELEE) {}
	public:
		virtual void Enter(AnimationStateMachine* animation_state_machine) override
		{

			animation_state_machine->SetAnimation("player_idle_hit_react.anim", 0.0f, true);
		}
		virtual void Exit(AnimationStateMachine* animation_state_machine) override
		{
			entt::entity owner_id = animation_state_machine->GetOwnerId();
			Player* player = SCENE_MGR->GetActor<Player>(owner_id);
			player->is_hit_ = false;
		}
		virtual void OnUpdate(AnimationStateMachine* animation_state_machine) override
		{
		}
	};

	class HitGRIdle : public AnimationState {
	public:
		HitGRIdle() : AnimationState(HIT_GR_IDLE) {}
	public:
		virtual void Enter(AnimationStateMachine* animation_state_machine) override
		{
			animation_state_machine->SetAnimation("player_idle_hit_react.anim", 0.0f, true);
		}
		virtual void Exit(AnimationStateMachine* animation_state_machine) override
		{
			entt::entity owner_id = animation_state_machine->GetOwnerId();
			Player* player = SCENE_MGR->GetActor<Player>(owner_id);
			player->is_hit_ = false;
		}
		virtual void OnUpdate(AnimationStateMachine* animation_state_machine) override
		{
		}
	};

	class HitGRAim : public AnimationState {
	public:
		HitGRAim() : AnimationState(HIT_GR_AIM) {}
	public:
		virtual void Enter(AnimationStateMachine* animation_state_machine) override
		{
			animation_state_machine->SetAnimation("player_gr_aim_pose_hit_react.anim", 0.0f, true);
		}
		virtual void Exit(AnimationStateMachine* animation_state_machine) override
		{
			entt::entity owner_id = animation_state_machine->GetOwnerId();
			Player* player = SCENE_MGR->GetActor<Player>(owner_id);
			player->is_hit_ = false;
		}
		virtual void OnUpdate(AnimationStateMachine* animation_state_machine) override
		{
		}
	};
};

class PlayerFullBodyAnimationStateMachine : public AnimationStateMachine {
public:
	enum States {
		NONE, 
		DODGE_ROLL,
		DIE,
		DEAD,
		ZOMBIE
	};

	PlayerFullBodyAnimationStateMachine(entt::entity owner_id, string skeletal_mesh_id, int range, string bone_name = "") : AnimationStateMachine(owner_id, skeletal_mesh_id, range, bone_name) {};

	virtual void OnInit() override {
		states_.insert({ NONE, make_shared<None>() });
		states_.insert({ DODGE_ROLL, make_shared<DodgeRoll>() });
		states_.insert({ DIE, make_shared<Die>() });
		states_.insert({ DEAD, make_shared<Dead>() });
		states_.insert({ ZOMBIE, make_shared<Zombie>() });

		// Dodge Roll
		{
			transitions_.insert({ NONE, Transition(DODGE_ROLL,[this](const AnimationStateMachine* animation_state_machine) {
				entt::entity owner_id = animation_state_machine->GetOwnerId();
				Player* player = SCENE_MGR->GetActor<Player>(owner_id);

				C_Movement* movement_component = SCENE_MGR->GetActor<Player>(owner_id)->GetMovementComponent();
				int* direction_vector = movement_component->accelaration_vector;

				bool is_movement_ = false;
				for (int i = 0;i < 3;i++) {
					if (direction_vector[i] != 0) {
						is_movement_ = true;
					}
				}

				if (player->roll_ == true && is_movement_ == true) {
					return true;
				}
				else {
					player->roll_ = false;
					return false;
				}
			})
			});

			transitions_.insert({ DODGE_ROLL, Transition(NONE,[this](const AnimationStateMachine* animation_state_machine) {
				if (IsAnimationEnded()) {
					return true;
				}
				else {
					return false;
				}
			})
			});
		}

		// Die
		{
			transitions_.insert({ NONE, Transition(DIE,[this](const AnimationStateMachine* animation_state_machine) {
				entt::entity owner_id = animation_state_machine->GetOwnerId();
				Player* player = SCENE_MGR->GetActor<Player>(owner_id);
				if (player->is_dead_) {
					return true;
				}
				else {
					return false;
				}
			})
			});
			transitions_.insert({ DIE, Transition(DEAD,[this](const AnimationStateMachine* animation_state_machine) {
				if (IsAnimationEnded()) {
					return true;
				}
				else {
					return false;
				}
			})
			});
		}

		{
			transitions_.insert({ NONE, Transition(ZOMBIE,[this](const AnimationStateMachine* animation_state_machine) {
				entt::entity owner_id = animation_state_machine->GetOwnerId();
				Player* player = SCENE_MGR->GetActor<Player>(owner_id);
				if (player->is_zombie_) {
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

	class DodgeRoll : public AnimationState {
	private:
		int roll_direction_[3];
	public:
		DodgeRoll() : AnimationState(DODGE_ROLL) {}
	public:
		virtual void Enter(AnimationStateMachine* animation_state_machine) override
		{
			entt::entity owner_id = animation_state_machine->GetOwnerId();

			Player* player = SCENE_MGR->GetActor<Player>(owner_id);
			player->is_rolling_ = true;
			player->controller_enable_ = false;

			C_Movement* movement_component = player->GetMovementComponent();
			player->GetStatus("max_speed")->PermanentVariation(100);

			for (int i = 0;i < 3;i++) {
				roll_direction_[i] = movement_component->accelaration_vector[i];
			}

			if (roll_direction_[0] == 1) {
				if (roll_direction_[2] == 1) {
					animation_state_machine->SetAnimation("player_dodge_roll_rf.anim", 0.2f, true);
				}
				else if (roll_direction_[2] == -1) {
					animation_state_machine->SetAnimation("player_dodge_roll_rb.anim", 0.2f, true);
				}
				else {
					animation_state_machine->SetAnimation("player_dodge_roll_r.anim", 0.2f, true);
				}
			}
			else if (roll_direction_[0] == -1) {
				if (roll_direction_[2] == 1) {
					animation_state_machine->SetAnimation("player_dodge_roll_lf.anim", 0.2f, true);
				}
				else if (roll_direction_[2] == -1) {
					animation_state_machine->SetAnimation("player_dodge_roll_lb.anim", 0.2f, true);
				}
				else {
					animation_state_machine->SetAnimation("player_dodge_roll_l.anim", 0.2f, true);
				}
			}
			else {
				if (roll_direction_[2] == 1) {
					animation_state_machine->SetAnimation("player_dodge_roll_f.anim", 0.2f, true);
				}
				else if(roll_direction_[2] == -1) {
					animation_state_machine->SetAnimation("player_dodge_roll_b.anim", 0.2f, true);
				}
			}
		}
		virtual void Exit(AnimationStateMachine* animation_state_machine) override
		{
			entt::entity owner_id = animation_state_machine->GetOwnerId();
			Player* player = SCENE_MGR->GetActor<Player>(owner_id);

			player->GetStatus("max_speed")->PermanentVariation(-100);
			player->controller_enable_ = true;
			player->is_rolling_ = false;
			player->roll_ = false;
		}
		virtual void OnUpdate(AnimationStateMachine* animation_state_machine) override
		{
			entt::entity owner_id = animation_state_machine->GetOwnerId();
			Player* player = SCENE_MGR->GetActor<Player>(owner_id);
			C_Movement* movement_component = player->GetMovementComponent();

			for (int i = 0;i < 3;i++) {
				movement_component->accelaration_vector[i] = roll_direction_[i];
			}
			movement_component->acceleration = 500.0f;
		}
	};

	class Die : public AnimationState {
	public:
		Die() : AnimationState(DIE) {}
	public:
		virtual void Enter(AnimationStateMachine* animation_state_machine) override
		{
			animation_state_machine->SetAnimation("player_die.anim", 0.2f, true);
		}
		virtual void Exit(AnimationStateMachine* animation_state_machine) override
		{
		}
		virtual void OnUpdate(AnimationStateMachine* animation_state_machine) override
		{
		}
	};

	class Dead : public AnimationState {
	public:
		Dead() : AnimationState(DEAD) {}
	public:
		virtual void Enter(AnimationStateMachine* animation_state_machine) override
		{
			animation_state_machine->SetAnimation("player_dead_pose.anim", 0.2f, true);
		}
		virtual void Exit(AnimationStateMachine* animation_state_machine) override
		{
		}
		virtual void OnUpdate(AnimationStateMachine* animation_state_machine) override
		{
		}
	};

	class Zombie : public AnimationState {
	public:
		Zombie() : AnimationState(ZOMBIE) {}
	public:
		virtual void Enter(AnimationStateMachine* animation_state_machine) override
		{
			animation_state_machine->SetAnimation("player_zombie_walk.anim", 0.5f, true);
			entt::entity owner_id = animation_state_machine->GetOwnerId();
			Player* player = SCENE_MGR->GetActor<Player>(owner_id);
			player->reg_scene_->try_get<C_StaticMesh>(owner_id)->static_mesh_id = "";

		}
		virtual void Exit(AnimationStateMachine* animation_state_machine) override
		{
		}
		virtual void OnUpdate(AnimationStateMachine* animation_state_machine) override
		{
			entt::entity owner_id = animation_state_machine->GetOwnerId();
			Player* player = SCENE_MGR->GetActor<Player>(owner_id);

			static float move_time = RandomFloatInRange(3.0f, 7.0f);
			static float rotation_angle = RandomFloatInRange(0.0f, 180.0f);
			player->GetMovementComponent()->accelaration_vector[2] = 1;
			player->GetMovementComponent()->max_speed = 20.0f;
			XMMATRIX rotation_matrix = XMMatrixRotationY(rotation_angle);
			player->transform_tree_.root_node->Rotate(*player->reg_scene_, owner_id, player->GetCurPosition(), rotation_matrix);
			if (move_time <= 0.0f) {
				move_time = RandomFloatInRange(3.0f, 7.0f);
				rotation_angle = RandomFloatInRange(0.0f, 180.0f);
				player->rotation_ = XMMatrixRotationY(rotation_angle);
			}

			move_time -= TM_DELTATIME;
		}
	};
};