#include "Player.h"
#include "Weapon.h"
#include "InGameScene.h"
#include "ItemObjects.h"
#include "Grenade.h"
#include "FX_BloodImpact.h"
#include "FX_ConcreteImpact.h"
#include "FX_Flame.h"
#include "FX_Muzzle.h"
#include "FX_Explosion.h"
#include "GameEvents.h"

using namespace reality;

void Player::OnInit(entt::registry& registry)
{
	Character::OnInit(registry);

	movement_component_->speed = 150;
	max_hp_ = cur_hp_ = 100;
	
	reality::C_SkeletalMesh skm;
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

	C_Socket socket_component;
	SkeletalMesh* skeletal_mesh = RESOURCE->UseResource<SkeletalMesh>(skm.skeletal_mesh_id);
	int skeleton_id = skeletal_mesh->skeleton.bone_name_id_map["Hand_R"];
	XMMATRIX socket_offset = XMMatrixRotationY(XMConvertToRadians(90))
		* XMMatrixRotationX(XMConvertToRadians(180))
		* XMMatrixTranslationFromVector({ -20, -4, 4, 0 });
	socket_component.AddSocket("RightHand", skeleton_id, XMMatrixRotationY(XMConvertToRadians(180)), socket_offset);
	registry.emplace<C_Socket>(entity_id_, socket_component);

	C_StaticMesh static_mesh_component;
	static_mesh_component.local = XMMatrixScalingFromVector({ 1.4f, 1.4f, 1.4f, 0.0f });
	static_mesh_component.world = XMMatrixIdentity();
	static_mesh_component.static_mesh_id = "SK_Handgun_01.stmesh";
	static_mesh_component.vertex_shader_id = "StaticMeshVS.cso";
	static_mesh_component.socket_name = "RightHand";
	registry.emplace<C_StaticMesh>(entity_id_, static_mesh_component);

	transform_tree_.root_node = make_shared<TransformTreeNode>(TYPE_ID(C_CapsuleCollision));
	transform_tree_.AddNodeToNode(TYPE_ID(C_CapsuleCollision), TYPE_ID(C_SkeletalMesh));
	transform_tree_.AddNodeToNode(TYPE_ID(C_CapsuleCollision), TYPE_ID(C_Camera));
	transform_tree_.AddNodeToNode(TYPE_ID(C_CapsuleCollision), TYPE_ID(C_SoundListener));
	transform_tree_.AddNodeToNode(TYPE_ID(C_SkeletalMesh), TYPE_ID(C_Socket));

	transform_matrix_ = XMMatrixTranslation(0, 100, 0);
	transform_tree_.root_node->OnUpdate(registry, entity_id_, transform_matrix_);

	C_SkeletalMesh* skm_ptr = registry.try_get<C_SkeletalMesh>(entity_id_);
	skm_ptr->local = XMMatrixScalingFromVector({ 0.3, 0.3, 0.3, 0.0 }) * XMMatrixRotationY(XMConvertToRadians(180));

	// create anim slot
	C_Animation animation_component;
	animation_component.SetBaseAnimObject<AnimationBase>();
	animation_component.AddNewAnimSlot<PlayerUpperBodyAnimationStateMachine>("UpperBody", skm.skeletal_mesh_id, "Spine_02", 6, entity_id_);
	reg_scene_->emplace_or_replace<reality::C_Animation>(entity_id_, animation_component);

	SetCharacterAnimation("A_TP_CH_Breathing_Anim_Retargeted_Unreal Take.anim");

	// FlashLight
	AddFlashLight();

	// Inventory
	inventory_.resize(INVENTORY_MAX);
	inventory_timer_.resize(INVENTORY_MAX);

	cur_hp_ = 0;

	// true means : this actor causes trigger event when overlaped at trigger 
	trigger_sensor = true;
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

	UpdateTimer();
}

void Player::SetCharacterAnimation(string anim_id, string anim_slot_id)
{
	reality::C_Animation* animation_component_ptr = reg_scene_->try_get<reality::C_Animation>(entity_id_);
	int slot_index = animation_component_ptr->name_to_anim_slot_index[anim_slot_id];
	if (animation_component_ptr->anim_slots[slot_index].second.anim_object_->GetCurAnimationId() != anim_id) {
		animation_component_ptr->anim_slots[slot_index].second.anim_object_->SetAnimation(anim_id, 0.2);
	}
	reg_scene_->emplace_or_replace<reality::C_Animation>(entity_id_, *animation_component_ptr);
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
	movement_component_->jump_pulse = 500.0f;
}

void Player::Idle()
{
	SetCharacterAnimation("A_TP_CH_Breathing_Anim_Retargeted_Unreal Take.anim");
}

void Player::Fire()
{
	if (is_aiming_ && !is_firing_) {

		is_firing_ = true;

		// Make Muzzle when Shot
		auto player_transform = GetTransformMatrix();
		XMVECTOR s, r, t;
		XMMatrixDecompose(&s, &r, &t, player_transform);
		EFFECT_MGR->SpawnEffect<FX_Muzzle>(t);

		// Make Shot Impact Effect
		auto ingame_scene = (InGameScene*)SCENE_MGR->GetScene(INGAME).get();
 		RayShape ray = ingame_scene->GetCameraSystem().CreateFrontRay();
		EVENT->PushEvent<AttackEvent>(vector<RayShape>{ray}, entity_id_);
	}
}

void Player::Aim()
{
	C_Camera* camera = reg_scene_->try_get<C_Camera>(entity_id_);
	if (is_aiming_ == false) {
		is_aiming_ = true;
		camera->target_rotation = 20;
		reg_scene_->emplace_or_replace<C_Camera>(entity_id_, *camera);
	}
	else {
		is_aiming_ = false;
		camera->target_rotation = 0;
		reg_scene_->emplace_or_replace<C_Camera>(entity_id_, *camera);
	}
}

void Player::ThrowGrenade()
{
	if (grenade_timer_ < grenade_cooltime_)
		return;

	grenade_timer_ -= grenade_cooltime_;

	auto grenade_entity = SCENE_MGR->AddActor<Grenade>();
	auto grenade_actor = SCENE_MGR->GetActor<Grenade>(grenade_entity); 
	XMVECTOR s, r, t;
	XMMatrixDecompose(&s, &r, &t, transform_matrix_);
	XMVECTOR pos = XMVectorAdd(t, XMVectorSet(0.0f, 50.0f, 0.0f, 0.0f));
	grenade_actor->SetPos(pos);
	XMVECTOR dir = XMVectorAdd(front_, XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
	grenade_actor->SetDir(dir, 4.0f);
}

bool Player::IsAiming()
{
	return is_aiming_;
}

void Player::ResetPos()
{
	spawn_point.m128_f32[1] += 100.f;
	transform_matrix_ = XMMatrixTranslationFromVector(spawn_point);
	transform_tree_.root_node->OnUpdate(*reg_scene_, entity_id_, transform_matrix_);
}

void Player::SetPos(const XMVECTOR& position)
{
	transform_matrix_ = XMMatrixTranslationFromVector(position);
	transform_tree_.root_node->Translate(*reg_scene_, entity_id_, transform_matrix_);
}

float Player::GetMaxHp() const
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

float Player::GetCurHp() const
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

void Player::UpdateTimer()
{
	// Grenade Timer
	if (grenade_timer_ < grenade_cooltime_)
		grenade_timer_ += TIMER->GetDeltaTime();

	// Inventory Timer
	for (int i = 0; i < inventory_.size(); i++)
	{
		if (inventory_[i] == NULL)
			continue;
		if (inventory_[i]->GetCooltime() > inventory_timer_[i])
		{
			inventory_timer_[i] += TIMER->GetDeltaTime();
		}

	}
}

bool Player::AcquireItem(shared_ptr<ItemBase> item)
{
	string item_id = typeid(*item.get()).name();
	
	// 1. If item exists in inventory.
	for (int i = 0; i < inventory_.size(); i++)
	{
		if (inventory_[i] == NULL)
			continue;

		if (item_id == typeid(*inventory_[i].get()).name())
		{
			inventory_[i]->AddCount(item->GetCount());
			return true;
		}
	}

	// 2. if Item doesn't exist in inventory, Check Inventory
	for (int i = 0; i < inventory_.size(); i++)
	{
		if (inventory_[i] == NULL)
		{
			inventory_[i] = item;
			item->SetOwner(this);
			inventory_timer_[i] = 0;
			return true;
		}
			
	}

	// 3. If Inventory was full, Can't Acquire Item
	return false;
}

void Player::UseItem(int slot)
{
	if (INVENTORY_MAX <= slot)
		return;

	if (inventory_[slot] == NULL)
		return;

	if (inventory_timer_[slot] < inventory_[slot]->GetCooltime())
		return;

	inventory_[slot]->Use();

	inventory_timer_[slot] = 0;

	if (inventory_[slot]->GetCount() == 0)
	{
		inventory_[slot] = NULL;
	}
}

void Player::PickClosestItem()
{
	if (selectable_items_.empty())
		return;

	auto closest_item = selectable_items_.begin();
	EVENT->PushEvent<DeleteActorEvent>(closest_item->second->entity_id_);
	selectable_items_.erase(closest_item);
}

vector<shared_ptr<ItemBase>>& Player::GetInventory()
{
	return inventory_;
}

vector<float>& Player::GetInventoryTimer()
{
	return inventory_timer_;
}

void Player::SetSpawnPoint(XMVECTOR point)
{
	spawn_point = point;
	ResetPos();
}
