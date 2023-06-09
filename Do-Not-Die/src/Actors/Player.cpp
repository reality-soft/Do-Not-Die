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
#include "AttackEvent.h"
#include "Player_ASM.h"

using namespace reality;

void Player::OnInit(entt::registry& registry)
{
	Character::OnInit(registry);
	tag = "player";

	AddStatus("hp", CharacterStatus(200, 200, 0, 200));
	AddStatus("gunfire_damage", CharacterStatus(30, 30, 30, 60));
	AddStatus("meele_damage", CharacterStatus(60, 60, 60, 120));
	AddStatus("max_speed", CharacterStatus(150, 150, 150, 300));
	AddStatus("infection", CharacterStatus(0, 0, 0, 100));

	GetMovementComponent()->speed = 0;
	GetMovementComponent()->acceleration = 300;
	GetMovementComponent()->max_speed = GetStatus("max_speed")->GetCurrentValue();

	//max_hp_ = cur_hp_ = 100;
	//damage_ = 30.0f;

	C_TriggerSensor trigger_sensor;
	trigger_sensor.can_sense_tags.insert("item");
	trigger_sensor.can_sense_tags.insert("extract");
	trigger_sensor.can_sense_tags.insert("repair");
	trigger_sensor.can_sense_tags.insert("defense");
	registry.emplace<reality::C_TriggerSensor>(entity_id_, trigger_sensor);

	reality::C_SkeletalMesh skm;
	skm.skeletal_mesh_id = "SM_Chr_Biker_Male.skmesh";
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
	XMMATRIX socket_offset = XMMatrixRotationZ(XMConvertToRadians(180))
		* XMMatrixRotationY(XMConvertToRadians(100))
		* XMMatrixTranslationFromVector({-15, -3, 5, 0 });
	socket_component.AddSocket("Rifle", skeleton_id, XMMatrixRotationY(XMConvertToRadians(180)), socket_offset);

	socket_offset = XMMatrixRotationZ(XMConvertToRadians(90)) 
		* XMMatrixRotationY(XMConvertToRadians(90))
		* XMMatrixRotationX(XMConvertToRadians(90))
		* XMMatrixTranslationFromVector({ -10, 0, 4, 0 });
	socket_component.AddSocket("Pistol", skeleton_id, XMMatrixRotationY(XMConvertToRadians(180)), socket_offset);

	socket_offset = XMMatrixRotationZ(XMConvertToRadians(90))
		* XMMatrixRotationY(XMConvertToRadians(90))
		* XMMatrixRotationX(XMConvertToRadians(90))
		* XMMatrixTranslationFromVector({ -10, 0, 4, 0 });
	socket_component.AddSocket("Axe", skeleton_id, XMMatrixRotationY(XMConvertToRadians(180)), socket_offset);

	socket_offset = XMMatrixRotationZ(XMConvertToRadians(90))
		* XMMatrixRotationY(XMConvertToRadians(90))
		* XMMatrixRotationX(XMConvertToRadians(90))
		* XMMatrixTranslationFromVector({ -10, 0, 4, 0 });
	socket_component.AddSocket("Grenade", skeleton_id, XMMatrixRotationY(XMConvertToRadians(180)), socket_offset);

	registry.emplace<C_Socket>(entity_id_, socket_component);

	socket_ids_[0] = "Rifle";
	socket_ids_[1] = "Pistol";
	socket_ids_[2] = "Axe";
	socket_ids_[3] = "Grenade";

	stm_ids_[0] = "WEP_AK47.stmesh";
	stm_ids_[1] = "WEP_Pistol.stmesh";
	stm_ids_[2] = "WEP_Axe.stmesh";
	stm_ids_[3] = "Grenade.stmesh";

	stm_local_[0] = XMMatrixScalingFromVector({ 0.8f, 0.8f, 0.8f, 0.8f });
	stm_local_[1] = XMMatrixScalingFromVector({ 0.8f, 0.8f, 0.8f, 0.8f });
	stm_local_[2] = XMMatrixIdentity();
	stm_local_[3] = XMMatrixIdentity();

	C_StaticMesh static_mesh_component;
	static_mesh_component.local = TransformR(XMFLOAT3(0, 180, 0));
	static_mesh_component.world = XMMatrixIdentity() * static_mesh_component.local;
	static_mesh_component.static_mesh_id = stm_ids_[static_cast<int>(cur_equipped_weapon_)];
	static_mesh_component.vertex_shader_id = "StaticMeshVS.cso";
	static_mesh_component.socket_name = socket_ids_[static_cast<int>(cur_equipped_weapon_)];
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
	animation_component.AddNewAnimSlot<PlayerUpperBodyAnimationStateMachine>("UpperBody", entity_id_, skm.skeletal_mesh_id, 7, "Spine_01");
	animation_component.AddNewAnimSlot<PlayerFullBodyAnimationStateMachine>("FullBody", entity_id_, skm.skeletal_mesh_id, 1, "Root");
	auto sm = (PlayerUpperBodyAnimationStateMachine*)animation_component.GetAnimSlotByName("UpperBody");
	sm->SetPlayer(this);
	reg_scene_->emplace_or_replace<reality::C_Animation>(entity_id_, animation_component);

	// FlashLight
	AddFlashLight();

	// Inventory
	inventory_.resize(INVENTORY_MAX);
	inventory_timer_.resize(INVENTORY_MAX);

	tag = "player";
}

void Player::OnUpdate()
{
	GetCapsuleComponent()->hit_enable = !(is_rolling_ || is_hit_);

	if (controller_enable_)
	{
		C_Camera* camera = reg_scene_->try_get<C_Camera>(entity_id_);
		rotation_ = XMMatrixRotationY(camera->pitch_yaw.y);
	}

	Character::OnUpdate();
	CalculateMovementAngle();
	SetCharacterMovementAnimation();
	ChangeWeapon();

	// FlashLight Update
	UpdateFlashLight();
	UpdateTimer();

	// Status
	GetMovementComponent()->max_speed = GetStatus("max_speed")->GetCurrentValue();
	IncreaseInfection();
	UpdateStatus();

	if (GetStatus("hp")->GetCurrentValue() <= 0) {
		EVENT->PushEvent<GameResultEvent>(GameResultType::ePlayerDead);
		controller_enable_ = false;
		is_dead_ = true;
	}

	if (GetStatus("infection")->GetCurrentValue() >= 100.f)
	{
		EVENT->PushEvent<GameResultEvent>(GameResultType::ePlayerInfected);
		controller_enable_ = false;
		is_zombie_ = true;
		return;
	}
}

void Player::SetCharacterMovementAnimation()
{
	reality::C_Animation* animation_component_ptr = reg_scene_->try_get<reality::C_Animation>(entity_id_);
	AnimationBase* anim_slot = animation_component_ptr->GetAnimSlotByName("Base");

	string anim_id = "player_idle.anim";

	vector<AnimNotify> notifies;
	
	if (GetMovementComponent()->speed >= 0.1f) {

		notifies.resize(2);
		notifies[0].event = make_shared<WalkEvent>();
		notifies[1].event = make_shared<WalkEvent>();

		if (angle_ >= 330.0f || angle_ < 30.0f) {
			anim_id = "player_jog_f.anim";
			notifies[0].frame = 8 * 2;
			notifies[1].frame = 18 * 2;
		}
		else if (30.0f <= angle_ && angle_ < 110.0f) {
			anim_id = "player_jog_rf.anim";
			notifies[0].frame = 8 * 2;
			notifies[1].frame = 18 * 2;
		}
		else if (110.0f <= angle_ && angle_ < 160.0f) {
			anim_id = "player_jog_rb.anim";
			notifies[0].frame = 6 * 2;
			notifies[1].frame = 17 * 2;
		}
		else if (160.0f <= angle_ && angle_ < 200.0f) {
			anim_id = "player_jog_b.anim";
			notifies[0].frame = 9 * 2;
			notifies[1].frame = 19 * 2;
		}
		else if (200.0f <= angle_ && angle_ < 250.0f) {
			anim_id = "player_jog_lb.anim";
			notifies[0].frame = 6 * 2;
			notifies[1].frame = 17 * 2;
		}
		else if (250.0f <= angle_ && angle_ < 330.0f) {
			anim_id = "player_jog_lf.anim";
			notifies[0].frame = 6 * 2;
			notifies[1].frame = 18 * 2;
		}
	}

	if (anim_slot->GetCurAnimationId() != anim_id) {
		anim_slot->SetAnimation(anim_id, 1.0, true, notifies);
	}
	reg_scene_->emplace_or_replace<reality::C_Animation>(entity_id_, *animation_component_ptr);
}

void Player::MoveRight()
{
	if (controller_enable_ == false)
		return;

	GetMovementComponent()->accelaration_vector[0] += 1;
}

void Player::MoveLeft()
{
	if (controller_enable_ == false)
		return;

	GetMovementComponent()->accelaration_vector[0] -= 1;
}

void Player::MoveForward()
{
	if (controller_enable_ == false)
		return;

	GetMovementComponent()->accelaration_vector[2] += 1;
}

void Player::MoveBack()
{
	if (controller_enable_ == false)
		return;

	GetMovementComponent()->accelaration_vector[2] -= 1;
}

void Player::Roll()
{
	if (controller_enable_ == false)
		return;
	if (GetMovementComponent()->jump_pulse > 1.0f || GetMovementComponent()->gravity_pulse > 1.0f) {
		return;
	}

	wchar_t output[32]; 
	swprintf(output, 32, L"jump_pulse: %f\n", GetMovementComponent()->jump_pulse);

	OutputDebugStringW(output);

	swprintf(output, 32, L"gravity_pulse: %f\n", GetMovementComponent()->gravity_pulse);
	OutputDebugStringW(output);

	roll_ = true;
}

void Player::Jump()
{
	if (controller_enable_ == false)
		return;

	if (GetMovementComponent()->jump_pulse <= 0 && GetMovementComponent()->gravity_pulse <= 0) {
		GetMovementComponent()->jump_pulse = 150.0f;
		EVENT->PushEvent<SoundGenerateEvent>(entity_id_, SFX, "S_CH_Jump_Start.wav", 1.0f, false);
	}
}

void Player::Attack()
{
	if (cur_equipped_weapon_ == EQUIPPED_WEAPON::GRENADE)
	{
		if (is_attacking_ || is_aiming_ == false)
			return;

		if (cur_weapon_using_remained_[(int)EQUIPPED_WEAPON::GRENADE] <= 0)
			return;

		is_attacking_ = true;

		return;
	}

	if (cur_equipped_weapon_ == EQUIPPED_WEAPON::MELEE_WEAPON)
	{
		is_attacking_ = true;
		//MeeleAttack();
		return;
	}
		
	if (is_aiming_ && !is_attacking_ && cur_weapon_using_remained_[static_cast<int>(cur_equipped_weapon_)] > 0) {
		cur_weapon_using_remained_[static_cast<int>(cur_equipped_weapon_)]--;
		is_attacking_ = true;

		// Make Muzzle when Shot
		XMVECTOR player_position = GetCurPosition();
		if (cur_equipped_weapon_ == EQUIPPED_WEAPON::AUTO_RIFLE)
		{
			player_position += front_ * 40.0f;
			player_position += right_ * -3.0f;
			player_position += XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f) * 40.0f;
		}
		else if (cur_equipped_weapon_ == EQUIPPED_WEAPON::HAND_GUN)
		{
			player_position += front_ * 35.0f;
			player_position += right_ * 6.0f;
			player_position += XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f) * 45.0f;
		}
		
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
		EVENT->PushEvent<AttackEvent_SingleRay>(ray, entity_id_);
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

void Player::Reload()
{
	if (is_reloading_ == false && cur_weapon_total_remained_[static_cast<int>(cur_equipped_weapon_)] > 0) {
		is_reloading_ = true;
	}
}

void Player::ThrowGrenade()
{
	cur_weapon_using_remained_[(int)EQUIPPED_WEAPON::GRENADE]--;

	auto grenade_entity = SCENE_MGR->AddActor<Grenade>();
	auto grenade_actor = SCENE_MGR->GetActor<Grenade>(grenade_entity); 
	XMVECTOR pos = cur_position_;
	pos += right_ * 5.0f;
	//pos += front_;
	pos += XMVectorSet(0.0f, 40.0f, 0.0f, 0.0f);
	grenade_actor->SetPos(pos);
	auto ingame_scene = (InGameScene*)SCENE_MGR->GetScene(INGAME).get();
	XMVECTOR dir = ingame_scene->GetCameraSystem().GetCamera()->look;
	grenade_actor->SetDir(dir, 15.0f);
}

void Player::MeeleAttack()
{
	is_attacking_ = true;
	C_CapsuleCollision* capsule_collision = GetCapsuleComponent();
	if (capsule_collision == nullptr)
		return;
	
	SphereShape attack_sphere;

	auto capsule_info = GetTipBaseAB(capsule_collision->capsule);
	XMVECTOR shepre_center = capsule_info[3] +(front_ * capsule_collision->capsule.radius * 3);
	attack_sphere.center = _XMFLOAT3(shepre_center);
	attack_sphere.radius = capsule_collision->capsule.radius * 2;

	EVENT->PushEvent<AttackEvent_BoundSphere>(GetStatus("meele_damage")->GetCurrentValue(), attack_sphere, entity_id_);
}

void Player::TakeDamage(int damage)
{
	is_hit_ = true;
	GetStatus("hp")->PermanentVariation(-damage);

	if (is_infected_ == true)
		return;

	++hit_count_;
	infection_probability_ = pow(hit_count_, 2);
	is_infected_ = Probability(infection_probability_);
};

void Player::IncreaseInfection()
{
	if (is_infected_ == false)
		return;

	static float timer = 0;
	timer += TM_DELTATIME;
	if (timer >= 1.0f)
	{
		GetStatus("infection")->PermanentVariation(0.5f);
		timer = 0.0f;
	}
}


bool Player::IsAiming()
{
	return is_aiming_;
}

bool Player::IsReloading()
{
	return is_reloading_;
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
	{
		flash_onoff = !flash_onoff;
		EVENT->PushEvent<SoundGenerateEvent>(entity_id_, SFX, "S_WEP_Flashlight_Click.wav", 1.0f, false);
	}

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
	XMVECTOR velocity = GetMovementComponent()->velocity;
	velocity.m128_f32[1] = 0;
	direction_ = XMVector3Transform(XMVector3Normalize(velocity), rotation_);
	float dot_product = XMVectorGetX(XMVector3Dot(front_, direction_));

	angle_ = XMVectorGetX(XMVector3AngleBetweenNormals(front_, direction_));

	if (XMVectorGetX(XMVector3Dot(right_, direction_)) < 0)
		angle_ = XM_2PI - angle_;
	
	angle_ = XMConvertToDegrees(angle_);
	angle_ += 0.2f;
}

void Player::ChangeWeapon()
{
	if (is_aiming_ == true || is_reloading_ || is_rolling_ == true || controller_enable_ == false) {
		return;
	}
	int cur_equipped_weapon = static_cast<int>(cur_equipped_weapon_);
	cur_equipped_weapon += DINPUT->GetMouseWheel() / 120 * -1;

	int num_of_weapon_type = static_cast<int>(EQUIPPED_WEAPON::NUM_OF_WEAPON_TYPE);
	if (cur_equipped_weapon >= num_of_weapon_type) {
		cur_equipped_weapon %= num_of_weapon_type;
	}
	if (cur_equipped_weapon < 0) {
		cur_equipped_weapon = num_of_weapon_type - 1;
	}

	C_StaticMesh* stm_ptr = reg_scene_->try_get<C_StaticMesh>(entity_id_);
	stm_ptr->socket_name = socket_ids_[cur_equipped_weapon];
	stm_ptr->static_mesh_id = stm_ids_[cur_equipped_weapon];
	stm_ptr->local = stm_local_[cur_equipped_weapon];

	cur_equipped_weapon_ = static_cast<EQUIPPED_WEAPON>(cur_equipped_weapon);
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

	bool getting_item_success = false;

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
	case ItemType::eVaccine:
	{
		shared_ptr<VaccineItem> vaccine = make_shared<VaccineItem>();
		vaccine->OnCreate();
		vaccine->AddCount(1);
		vaccine->item_type_ = closest_item->second->item_type_;
		getting_item_success = AcquireItem(vaccine);
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
