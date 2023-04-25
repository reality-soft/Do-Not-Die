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
#include "BlendSpace2D.h"

using namespace reality;

void Player::OnInit(entt::registry& registry)
{
	Character::OnInit(registry);

	movement_component_->speed = 0;
	movement_component_->acceleration = 300;
	movement_component_->max_speed = 150;
	max_hp_ = cur_hp_ = 100;
	
	C_TriggerSensor trigger_sensor;
	trigger_sensor.can_sense_tags.insert("item");
	trigger_sensor.can_sense_tags.insert("extract");
	trigger_sensor.can_sense_tags.insert("repair");
	trigger_sensor.can_sense_tags.insert("defense");
	registry.emplace<reality::C_TriggerSensor>(entity_id_, trigger_sensor);

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

	C_SoundGenerator sound_generator;
	sound_generator.local;
	registry.emplace<C_SoundGenerator>(entity_id_, sound_generator);

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
	static_mesh_component.local = XMMatrixScalingFromVector({ 1.4f, 1.4f, 1.4f, 1.0f });
	static_mesh_component.world = XMMatrixIdentity() * static_mesh_component.local;
	static_mesh_component.static_mesh_id = "SK_Handgun_01.stmesh";
	static_mesh_component.vertex_shader_id = "StaticMeshVS.cso";
	static_mesh_component.socket_name = "RightHand";
	registry.emplace<C_StaticMesh>(entity_id_, static_mesh_component);

	transform_tree_.root_node = make_shared<TransformTreeNode>(TYPE_ID(C_CapsuleCollision));
	transform_tree_.AddNodeToNode(TYPE_ID(C_CapsuleCollision), TYPE_ID(C_SkeletalMesh));
	transform_tree_.AddNodeToNode(TYPE_ID(C_CapsuleCollision), TYPE_ID(C_Camera));
	transform_tree_.AddNodeToNode(TYPE_ID(C_CapsuleCollision), TYPE_ID(C_SoundListener));
	transform_tree_.AddNodeToNode(TYPE_ID(C_CapsuleCollision), TYPE_ID(C_SoundGenerator));
	transform_tree_.AddNodeToNode(TYPE_ID(C_SkeletalMesh), TYPE_ID(C_Socket));

	C_SkeletalMesh* skm_ptr = registry.try_get<C_SkeletalMesh>(entity_id_);
	skm_ptr->local = XMMatrixScalingFromVector({ 0.3, 0.3, 0.3, 1.0 }) * XMMatrixRotationY(XMConvertToRadians(180));

	cur_position_ = { 0.0f, 100.0f, 0.0f, 0.0f };
	transform_tree_.root_node->OnUpdate(registry, entity_id_, XMMatrixTranslationFromVector(cur_position_));
	
	C_Animation animation_component(skeletal_mesh->skeleton.id_bone_map.size());
	animation_component.SetBaseAnimObject<AnimationBase>(skm.skeletal_mesh_id, 0);
	animation_component.GetAnimSlotByName("Base")->SetAnimation("A_TP_CH_Breathing_Anim_Retargeted_Unreal Take.anim", 0.5);
	animation_component.AddNewAnimSlot<PlayerUpperBodyAnimationStateMachine>("UpperBody", entity_id_, skm.skeletal_mesh_id, 6, "Spine_01");
	reg_scene_->emplace_or_replace<reality::C_Animation>(entity_id_, animation_component);

	// FlashLight
	AddFlashLight();

	// Inventory
	inventory_.resize(INVENTORY_MAX);
	inventory_timer_.resize(INVENTORY_MAX);
	
	cur_hp_ = 0;
	tag = "player";


}

void Player::SetCharacterMovementAnimation()
{
	reality::C_Animation* animation_component_ptr = reg_scene_->try_get<reality::C_Animation>(entity_id_);
	AnimationBase* anim_slot = animation_component_ptr->GetAnimSlotByName("Base");

	string anim_id = "A_TP_CH_Breathing_Anim_Retargeted_Unreal Take.anim";

	if (movement_component_->speed >= 0.1f) {
		if (angle_ >= 330.0f || angle_ < 30.0f) {
			anim_id = "A_TP_CH_Jog_F_Anim_Retargeted_Unreal Take.anim";
		}
		else if (30.0f <= angle_ && angle_ < 110.0f) {
			anim_id = "A_TP_CH_Jog_RF_Anim_Retargeted_Unreal Take.anim";
		}
		else if (110.0f <= angle_ && angle_ < 160.0f) {
			anim_id = "A_TP_CH_Jog_RB_Anim_Retargeted_Unreal Take.anim";
		}
		else if (160.0f <= angle_ && angle_ < 200.0f) {
			anim_id = "A_TP_CH_Jog_B_Anim_Retargeted_Unreal Take.anim";
		}
		else if (200.0f <= angle_ && angle_ < 250.0f) {
			anim_id = "A_TP_CH_Jog_LB_Anim_Retargeted_Unreal Take.anim";
		}
		else if (250.0f <= angle_ && angle_ < 330.0f) {
			anim_id = "A_TP_CH_Jog_LF_Anim_Retargeted_Unreal Take.anim";
		}
	}

	if (anim_slot->GetCurAnimationId() != anim_id) {
		anim_slot->SetAnimation(anim_id, 1.0);
	}
	reg_scene_->emplace_or_replace<reality::C_Animation>(entity_id_, *animation_component_ptr);
}

void Player::OnUpdate()
{
	if (controller_enable_)
	{
		C_Camera* camera = reg_scene_->try_get<C_Camera>(entity_id_);
		rotation_ = XMMatrixRotationY(camera->pitch_yaw.y);
	}

	Character::OnUpdate();
	CalculateMovementAngle();
	SetCharacterMovementAnimation();

	// FlashLight Update
	UpdateFlashLight();

	UpdateTimer();
}

void Player::MoveRight()
{
	if (controller_enable_ == false)
		return;

	movement_component_->accelaration_vector[0] += 1;
}

void Player::MoveLeft()
{
	if (controller_enable_ == false)
		return;

	movement_component_->accelaration_vector[0] -= 1;
}

void Player::MoveForward()
{
	if (controller_enable_ == false)
		return;

	movement_component_->accelaration_vector[2] += 1;
}

void Player::MoveBack()
{
	if (controller_enable_ == false)
		return;

	movement_component_->accelaration_vector[2] -= 1;
}

void Player::Jump()
{
	if (controller_enable_ == false)
		return;

	//if (movement_component_->jump_pulse <= 0 && movement_component_->gravity_pulse <= 0) {
		movement_component_->jump_pulse = 150.0f;
	//}
}

void Player::Fire()
{
	if (is_aiming_ && !is_firing_) {

		is_firing_ = true;

		// Make Muzzle when Shot
		XMVECTOR player_position = GetCurPosition();
		player_position += front_ * 30.0f;
		player_position += right_ * 6.0f;
		player_position += XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f) * 40.0f;
		EFFECT_MGR->SpawnEffect<FX_Muzzle>(player_position);

		// Make Shot Sound when Shot
		auto& generator = reg_scene_->get<C_SoundGenerator>(GetEntityId());
		SoundQueue queue;
		queue.sound_filename = "S_WEP_Fire_001.wav";
		queue.sound_type = SoundType::SFX;
		queue.sound_volume = 0.5f;
		queue.is_looping = false;
		generator.sound_queue_list.push(queue);

		// Make Shot Impact Effect
		auto ingame_scene = (InGameScene*)SCENE_MGR->GetScene(INGAME).get();
 		RayShape ray = ingame_scene->GetCameraSystem().CreateFrontRay();
		EVENT->PushEvent<AttackEvent>(vector<RayShape>{ray}, entity_id_);
	}
}

void Player::Aim(bool active)
{
	C_Camera* camera = reg_scene_->try_get<C_Camera>(entity_id_);
	if (camera == nullptr)
		return;

	if (active)
		camera->target_rotation = 20;
	else
		camera->target_rotation = 0;

	is_aiming_ = active;
}

void Player::ThrowGrenade()
{
	if (grenade_timer_ < grenade_cooltime_)
		return;

	grenade_timer_ -= grenade_cooltime_;

	auto grenade_entity = SCENE_MGR->AddActor<Grenade>();
	auto grenade_actor = SCENE_MGR->GetActor<Grenade>(grenade_entity); 
	XMVECTOR pos = XMVectorAdd(cur_position_, XMVectorSet(0.0f, 50.0f, 0.0f, 0.0f));
	grenade_actor->SetPos(pos);
	XMVECTOR dir = XMVectorAdd(front_, XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
	grenade_actor->SetDir(dir, 4.0f);
}

bool Player::IsAiming()
{
	return is_aiming_;
}

void Player::InteractionRotate(XMVECTOR interaction_pos)
{
	controller_enable_ = false;
	
	XMVECTOR player_pos = GetCurPosition();
	XMVECTOR direction = XMVector3Normalize(interaction_pos - player_pos);
	float angle = XMVector3AngleBetweenVectors(XMVectorSet(0, 0, 1, 0), direction).m128_f32[0];
	if (XMVectorGetX(XMVector3Dot(XMVectorSet(1, 0, 0, 0), direction)) < 0)
		angle *= -1.0f;

	XMMATRIX rotation_matrix = XMMatrixRotationY(angle);
	transform_tree_.root_node->Rotate(*reg_scene_, entity_id_, player_pos, rotation_matrix);
}

void Player::ResetPos()
{
	spawn_point.m128_f32[1] += 100.f;
	cur_position_ = spawn_point;
	transform_tree_.root_node->OnUpdate(*reg_scene_, entity_id_, XMMatrixTranslationFromVector(cur_position_));
}

void Player::SetPos(const XMVECTOR& position)
{
	cur_position_ = position;
	transform_tree_.root_node->Translate(*reg_scene_, entity_id_, XMMatrixTranslationFromVector(cur_position_));
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

void Player::CalculateMovementAngle()
{
	XMVECTOR velocity = movement_component_->velocity;
	velocity.m128_f32[1] = 0;
	direction_ = XMVector3Transform(XMVector3Normalize(velocity), rotation_);
	float dot_product = XMVectorGetX(XMVector3Dot(front_, direction_));

	angle_ = XMVectorGetX(XMVector3AngleBetweenNormals(front_, direction_));

	if (XMVectorGetX(XMVector3Dot(right_, direction_)) < 0)
		angle_ = XM_2PI - angle_;
	
	angle_ = XMConvertToDegrees(angle_);
	angle_ += 0.2f;
}

void Player::UpdateTimer()
{
	// Grenade Timerdd
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

void Player::SelectSlot(int slot)
{
	selected_slot = slot;
}

void Player::DropItem()
{
	if (INVENTORY_MAX <= selected_slot)
		return;

	ItemBase* item_base = inventory_[selected_slot].get();
	if (item_base == nullptr)
		return;

	SCENE_MGR->AddActor<Item>(item_base->item_type_, _XMFLOAT3(GetCurPosition()), 30);
	item_base->Drop();
	inventory_timer_[selected_slot] = 0;
	if (item_base->GetCount() == 0)
	{
		inventory_[selected_slot] = NULL;
	}	
}

void Player::UseItem()
{
	if (INVENTORY_MAX <= selected_slot)
		return;

	ItemBase* item_base = inventory_[selected_slot].get();
	if (item_base == nullptr)
		return;

	if (item_base->item_type_ == ItemType::eRepairPart)
		return;

	if (inventory_timer_[selected_slot] < item_base->GetCooltime())
		return;

	item_base->Use();

	inventory_timer_[selected_slot] = 0;

	if (item_base->GetCount() == 0)
	{
		inventory_[selected_slot] = NULL;
	}

}

bool Player::HasRepairPart()
{
	for (int i = 0; i < 4; ++i)
	{
		if (inventory_[i] == nullptr)
			continue;

		if (inventory_[i]->item_type_ == ItemType::eRepairPart)
			return true;
	}
	return false;
}

void Player::UseRepairPart()
{
	for (auto& item : inventory_)
	{
		if (item == nullptr)
			continue;

		if (item.get()->item_type_ == ItemType::eRepairPart)
		{
			item->Use();
			if (item->GetCount() == 0)
			{
				item = nullptr;
			}
			break;
		}
	}
}

void Player::PickClosestItem()
{
	if (selectable_items_.empty())
		return;

	bool getting_item_success = false;;

	auto closest_item = selectable_items_.begin();
	switch (closest_item->second->item_type_)
	{
	case ItemType::eMedicalBox:
	{
		shared_ptr<MedicalBoxItem> medical_box = make_shared<MedicalBoxItem>();
		medical_box->OnCreate();
		medical_box->AddCount(1);
		medical_box->item_type_ = closest_item->second->item_type_;
		getting_item_success = AcquireItem(medical_box);
		break;
	}
	case ItemType::eHealKit:
	{
		shared_ptr<HealKitItem> heal_kit = make_shared<HealKitItem>();
		heal_kit->OnCreate();
		heal_kit->AddCount(1);
		heal_kit->item_type_ = closest_item->second->item_type_;
		getting_item_success = AcquireItem(heal_kit);
		break;
	}
	case ItemType::eEnergyDrink:
	{
		shared_ptr<EnergyDrinkItem> energy_drink = make_shared<EnergyDrinkItem>();
		energy_drink->OnCreate();
		energy_drink->AddCount(1);
		energy_drink->item_type_ = closest_item->second->item_type_;
		getting_item_success = AcquireItem(energy_drink);
		break;
	}
	case ItemType::eDrug:
	{
		shared_ptr<DrugItem> drug = make_shared<DrugItem>();
		drug->OnCreate();
		drug->AddCount(1);
		drug->item_type_ = closest_item->second->item_type_;
		getting_item_success = AcquireItem(drug);
		break;
	}
	case ItemType::eAR_Ammo:
	{
		shared_ptr<ARAmmoItem> ar_ammo = make_shared<ARAmmoItem>();
		ar_ammo->OnCreate();
		ar_ammo->AddCount(1);
		ar_ammo->item_type_ = closest_item->second->item_type_;
		getting_item_success = AcquireItem(ar_ammo);
		break;
	}
	case ItemType::ePistol_Ammo:
	{
		shared_ptr<PistolAmmoItem> pistol_ammo = make_shared<PistolAmmoItem>();
		pistol_ammo->OnCreate();
		pistol_ammo->AddCount(1);
		pistol_ammo->item_type_ = closest_item->second->item_type_;
		getting_item_success = AcquireItem(pistol_ammo);
		break;
	}
	case ItemType::eGrenade:
	{
		shared_ptr<GrenadeItem> grenade = make_shared<GrenadeItem>();
		grenade->OnCreate();
		grenade->AddCount(1);
		grenade->item_type_ = closest_item->second->item_type_;
		getting_item_success = AcquireItem(grenade);
		break;
	}
	case ItemType::eRepairPart:
	{
		shared_ptr<RepairPartItem> repair_part = make_shared<RepairPartItem>();
		repair_part->OnCreate();
		repair_part->AddCount(1);
		repair_part->item_type_ = closest_item->second->item_type_;
		getting_item_success = AcquireItem(repair_part);
		break;
	}
	}

	if (getting_item_success)
	{
		EVENT->PushEvent<DeleteActorEvent>(closest_item->second->entity_id_);
		selectable_items_.erase(closest_item);
		selectable_counts_--;
	}
	
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
