#include "Player.h"
#include "Weapon.h"

using namespace reality;

void Player::OnInit(entt::registry& registry)
{
	Character::OnInit(registry);

	movement_component_->speed = 150;
	max_hp_ = cur_hp_ = 100;

	SetCharacterAnimation("A_TP_CH_Breathing_Anim_Retargeted_Unreal Take.anim");

	reality::C_SkeletalMesh skm;
	skm.local = XMMatrixIdentity();
	skm.world = XMMatrixIdentity();
	skm.skeletal_mesh_id = "SM_Chr_Biker_Male_01.skmesh";
	skm.vertex_shader_id = "SkinningVS.cso";
	registry.emplace_or_replace<reality::C_SkeletalMesh>(entity_id_, skm);

	reality::C_CapsuleCollision capsule;
	capsule.SetCapsuleData(XMFLOAT3(0, 0, 0), 50, 15);
	registry.emplace<reality::C_CapsuleCollision>(entity_id_, capsule);

	C_Camera camera;
	camera.SetLocalFrom(capsule, 50);
	registry.emplace<C_Camera>(entity_id_, camera);

	C_SoundListener sound_listener;
	sound_listener.local = camera.local;
	registry.emplace<C_SoundListener>(entity_id_, sound_listener);


	transform_tree_.root_node = make_shared<TransformTreeNode>(TYPE_ID(reality::C_CapsuleCollision));
	transform_tree_.AddNodeToNode(TYPE_ID(C_CapsuleCollision), TYPE_ID(C_SkeletalMesh));
	transform_tree_.AddNodeToNode(TYPE_ID(C_CapsuleCollision), TYPE_ID(C_Camera));
	transform_tree_.AddNodeToNode(TYPE_ID(C_CapsuleCollision), TYPE_ID(C_SoundListener));

	transform_matrix_ = XMMatrixTranslation(0, 100, 0);
	transform_tree_.root_node->OnUpdate(registry, entity_id_, transform_matrix_);

	reality::C_SkeletalMesh* skm_ptr = registry.try_get<C_SkeletalMesh>(entity_id_);
	skm_ptr->local = XMMatrixRotationY(XMConvertToRadians(180)) * XMMatrixScalingFromVector({ 0.3, 0.3, 0.3, 0.0 });

	// weapon
	entt::entity weapon_id = SCENE_MGR->AddActor<Weapon>(entity_id_);
	SkeletalMesh* skeletal_mesh = RESOURCE->UseResource<SkeletalMesh>(skm.skeletal_mesh_id);
	int skeleton_id = skeletal_mesh->skeleton.bone_name_id_map["Hand_R"];
	Weapon* weapon = SCENE_MGR->GetActor<Weapon>(weapon_id);
	weapon->SetSocket(skeleton_id);
	weapon->SetOwnerTransform(skm_ptr->local);

	// create anim slot
	C_Animation animation;
	animation.AddNewAnimSlot("UpperBody", skm.skeletal_mesh_id, "Spine_02", 6);
	reg_scene_->emplace_or_replace<reality::C_Animation>(entity_id_, animation);
	reg_scene_->emplace_or_replace<reality::C_Animation>(entity_id_, animation);

	// FlashLight
	AddFlashLight();
}

void Player::OnUpdate()
{
	C_Camera* camera = reg_scene_->try_get<C_Camera>(entity_id_);
	XMVECTOR scale, rotation, translation;
	XMMatrixDecompose(&scale, &rotation, &translation, transform_matrix_);
	XMMATRIX rotation_matrix = XMMatrixRotationY(camera->pitch_yaw.y);
	transform_tree_.root_node->Rotate(*reg_scene_, entity_id_, translation, rotation_matrix);
	front_ = XMVector3Transform({ 0, 0, 1, 0 }, rotation_matrix);
	right_ = XMVector3Transform({ 1, 0, 0, 0 }, rotation_matrix);

	// FlashLight Update
	UpdateFlashLight();
}

void Player::SetCharacterAnimation(string anim_id, string anim_slot_id)
{
	if (anim_slot_id == "") {
		C_Animation* animation_component = reg_scene_->try_get<reality::C_Animation>(entity_id_);
		if (animation_component == nullptr || animation_component->anim_id == anim_id) {
			return;
		}

		animation_component->anim_id = anim_id;
	}
	else {
		C_Animation* animation_component = reg_scene_->try_get<reality::C_Animation>(entity_id_);
		if (animation_component == nullptr) {
			return;
		}

		int anim_slot_index = animation_component->name_to_anim_slot_index[anim_slot_id];
		AnimSlot& anim_slot = animation_component->anim_slots[anim_slot_index].second;
		if (anim_slot.anim_id == anim_id) {
			return;
		}

		anim_slot.anim_id = anim_id;
	}
}

void Player::MoveRight()
{
	SetCharacterAnimation("A_TP_CH_Jog_RF_Anim_Retargeted_Unreal Take.anim");
	movement_component_->direction += right_;
}

void Player::MoveRightForward()
{
	SetCharacterAnimation("A_TP_CH_Jog_RF_Anim_Retargeted_Unreal Take.anim");
	movement_component_->direction += front_;
	movement_component_->direction += right_;
}

void Player::MoveRightBack()
{
	SetCharacterAnimation("A_TP_CH_Jog_RB_Anim_Retargeted_Unreal Take.anim");
	movement_component_->direction -= front_;
	movement_component_->direction += right_;
}

void Player::MoveLeft()
{
	SetCharacterAnimation("A_TP_CH_Jog_LF_Anim_Retargeted_Unreal Take.anim");
	movement_component_->direction -= right_;
}

void Player::MoveLeftForward()
{
	SetCharacterAnimation("A_TP_CH_Jog_LF_Anim_Retargeted_Unreal Take.anim");
	movement_component_->direction += front_;
	movement_component_->direction -= right_;
}

void Player::MoveLeftBack()
{
	SetCharacterAnimation("A_TP_CH_Jog_LB_Anim_Retargeted_Unreal Take.anim");
	movement_component_->direction -= front_;
	movement_component_->direction -= right_;
}

void Player::MoveForward()
{
	SetCharacterAnimation("A_TP_CH_Jog_F_Anim_Retargeted_Unreal Take.anim");
	movement_component_->direction += front_;
}

void Player::MoveBack()
{
	SetCharacterAnimation("A_TP_CH_Jog_B_Anim_Retargeted_Unreal Take.anim");
	movement_component_->direction -= front_;
}

void Player::Jump()
{
	movement_component_->jump_scale = 1000.0f;
	movement_state_ = MovementState::GRAVITY_FALL;
}

void Player::Idle()
{
	SetCharacterAnimation("A_TP_CH_Breathing_Anim_Retargeted_Unreal Take.anim");
}

void Player::Fire()
{
	SetCharacterAnimation("A_TP_CH_Handgun_Fire_Anim_Retargeted_Unreal Take.anim", "UpperBody");
}

void Player::ResetPos()
{
	transform_matrix_ = XMMatrixTranslationFromVector({ 0.f, 100.f, 0.f, 0.f });
	transform_tree_.root_node->OnUpdate(SCENE_MGR->GetRegistry(), entity_id_, transform_matrix_);
}

void Player::SetPos(const XMVECTOR& position)
{
	transform_matrix_ = XMMatrixTranslationFromVector(position);
	transform_tree_.root_node->Translate(SCENE_MGR->GetRegistry(), entity_id_, transform_matrix_);
}

int Player::GetMaxHp() const
{
	return max_hp_;
}

void Player::SetCurHp(int hp)
{
	cur_hp_ = hp;
}

void Player::TakeDamage(int damage)
{
	cur_hp_ -= damage;
}

int Player::GetCurHp() const
{
	return cur_hp_;
}

void Player::AddFlashLight()
{
	auto& spot_light_comp = reg_scene_->emplace<C_SpotLight>(entity_id_);

	spot_light_comp.world = XMMatrixIdentity();
	spot_light_comp.local = XMMatrixIdentity();

	spot_light_comp.spot_light_id = "SL_FlashLight";
	spot_light_comp.spot_light = *(SpotLight*)RESOURCE->UseResource<BaseLight>("SL_FlashLight");

	/*spot_light_comp.light_color = { 1.0f, 1.0f, 1.0f, 1.0f };
	spot_light_comp.lifetime = -1.0f;
	spot_light_comp.timer = 0.0f;
	spot_light_comp.attenuation = { 1.0f, 0.0f, 0.0f };
	spot_light_comp.attenuation_level = { 1.0f, 0.0f, 0.0f };
	spot_light_comp.specular = 0.2f;
	spot_light_comp.range = 1000000.0f;
	spot_light_comp.spot = 8.0f;*/

	//auto& camera = reg_scene_->get<C_Camera>(entity_id_);
	//XMVECTOR S, R, T;
	//XMMatrixDecompose(&S, &R, &T, camera.world);
	//XMStoreFloat3(&spot_light_comp.position, T);
	//XMStoreFloat3(&spot_light_comp.direction, camera.look);

	transform_tree_.AddNodeToNode(TYPE_ID(C_Camera), TYPE_ID(C_SpotLight));

	transform_tree_.root_node->OnUpdate(*reg_scene_, entity_id_, transform_matrix_);
}

void Player::UpdateFlashLight()
{
	static bool flash_onoff = false;

	if (DINPUT->GetKeyState(DIK_F) == KEY_PUSH)
		flash_onoff = !flash_onoff;

	auto& spot_light_comp = reg_scene_->get<C_SpotLight>(entity_id_);

	static float range = spot_light_comp.spot_light.range;

	if (flash_onoff)
	{
		auto& capsule = reg_scene_->get<C_CapsuleCollision>(entity_id_);
		auto& camera = reg_scene_->get<C_Camera>(entity_id_);
		XMVECTOR s, r, t;
		XMMatrixDecompose(&s, &r, &t, capsule.world);

		XMVECTOR light_scale, light_rot_q, light_translate;
		XMMatrixDecompose(&light_scale, &light_rot_q, &light_translate, spot_light_comp.world);
		auto rotation_quaternion = DirectX::XMQuaternionRotationRollPitchYaw(camera.pitch_yaw.x, camera.pitch_yaw.y, 0);
		auto rotate_mat = XMMatrixRotationQuaternion(rotation_quaternion);
		XMFLOAT3 direction = { 0.0f, 0.0f, 1.0f };

		t = XMVectorAdd(t, XMVectorSet(0.0f, 40.0f, 0.0f, 0.0f));
		spot_light_comp.world = XMMatrixScalingFromVector(light_scale) 
			* rotate_mat 
			* XMMatrixTranslationFromVector(t);
		spot_light_comp.spot_light.range = range;
	}
	else
	{
		spot_light_comp.spot_light.range = 0.0f;
	}
	
}
