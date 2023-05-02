#include "GeneralZombie.h"
#include "AttackEvent.h"
#include "AnimationStateMachine.h"
#include "Player.h"
#include "Enemy_ASM.h"
#include "Enemy_BTN.h"

using namespace reality;

void GeneralZombie::OnInit(entt::registry& registry)
{
	Character::OnInit(registry);
	tag = "enemy";

	// setting character data
	movement_component_->max_speed = 100;
	movement_component_->acceleration = 100;
	max_hp_ = cur_hp_ = 100;
	damage_ = 10.0f;

	// set trigger sensor
	C_TriggerSensor trigger_sensor;
	trigger_sensor.can_sense_tags.insert("defense");
	registry.emplace<reality::C_TriggerSensor>(entity_id_, trigger_sensor);

	// setting character objects
	reality::C_SkeletalMesh skm;
	skm.local = XMMatrixIdentity();
	skm.world = XMMatrixIdentity();
	skm.skeletal_mesh_id = "Zombie_Businessman_Male_01.skmesh";
	skm.vertex_shader_id = "SkinningVS.cso";
	registry.emplace_or_replace<reality::C_SkeletalMesh>(entity_id_, skm);

	reality::C_CapsuleCollision capsule;
	capsule.tag = "enemy";
	capsule.SetCapsuleData(XMFLOAT3(0, 0, 0), 50, 10);
	registry.emplace<reality::C_CapsuleCollision>(entity_id_, capsule);

	transform_tree_.root_node = make_shared<TransformTreeNode>(TYPE_ID(reality::C_CapsuleCollision));
	transform_tree_.AddNodeToNode(TYPE_ID(C_CapsuleCollision), TYPE_ID(C_SkeletalMesh));

	reality::C_SkeletalMesh* skm_ptr = registry.try_get<C_SkeletalMesh>(entity_id_);
	skm_ptr->local = XMMatrixScalingFromVector({ 0.3, 0.3, 0.3, 0.0 }) * XMMatrixRotationY(XMConvertToRadians(180.f));

	cur_position_ = { 0.f, 100.f, 0.f, 0.f };
	transform_tree_.root_node->OnUpdate(registry, entity_id_, XMMatrixTranslationFromVector(cur_position_));

	SkeletalMesh* skeletal_mesh = RESOURCE->UseResource<SkeletalMesh>(skm.skeletal_mesh_id);
	C_Animation animation_component(skeletal_mesh->skeleton.id_bone_map.size());
	animation_component.SetBaseAnimObject<ZombieBaseAnimationStateMachine>(entity_id_, skm.skeletal_mesh_id, 0);
	animation_component.AddNewAnimSlot<ZombieUpperBodyAnimationStateMachine>("UpperBody", entity_id_, skm.skeletal_mesh_id, 3, "Spine_02");
	reg_scene_->emplace_or_replace<reality::C_Animation>(entity_id_, animation_component);
}

void GeneralZombie::OnUpdate()
{
	ChasePlayer();
	behavior_tree_.Update();
	Character::OnUpdate();
}

void GeneralZombie::SetCharacterAnimation(string anim_id) const
{
	reality::C_Animation* animation_component_ptr = reg_scene_->try_get<reality::C_Animation>(entity_id_);
	int base_index = animation_component_ptr->name_to_anim_slot_index["Base"];
	animation_component_ptr->anim_slots[base_index].second->SetAnimation(anim_id, 0.3);
	reg_scene_->emplace_or_replace<reality::C_Animation>(entity_id_, *animation_component_ptr);
}

void GeneralZombie::Jump()
{
}

void GeneralZombie::Idle()
{
}

void GeneralZombie::Attack()
{
	if (is_attacking_ == true)
		return;

	auto c_enemy_capsule = reg_scene_->try_get<C_CapsuleCollision>(entity_id_);
	if (c_enemy_capsule == nullptr)
		return;

	RayShape attack_ray;
	attack_ray.start = _XMFLOAT3(GetTipBaseAB(c_enemy_capsule->capsule)[3]);
	attack_ray.end = _XMFLOAT3((_XMVECTOR3(attack_ray.start) + (front_ * attack_distance_)));
	EVENT->PushEvent<AttackEvent_SingleRay>(attack_ray, entity_id_);
}

float GeneralZombie::GetMaxHp() const
{
	return max_hp_;
}

void GeneralZombie::SetCurHp(int hp)
{
}

void GeneralZombie::TakeDamage(int damage)
{
	is_hit_ = true;
	cur_hp_ -= damage;
}

void GeneralZombie::AddImpulse(XMVECTOR direction, float strength)
{
	CancelMovement();
	auto c_capsule = GetCapsuleComponent();
	if (c_capsule)
	{
		c_capsule->impulse_vector = direction;
		c_capsule->impulse_strength = strength;	
		GetMovementComponent()->gravity_pulse = 0;
	}
}

void GeneralZombie::SetMovement(const XMVECTOR& direction)
{
	if (XMVector3Length(direction).m128_f32[0] <= 0.00001f) {
		return;
	}


	XMVECTOR dir = direction; dir.m128_f32[1] = 0.0f;

	is_moving_ = true;
	XMVECTOR front = { 0.0f, 0.0f, 1.0f, 0.0f };
	XMVECTOR right = { 1.0f, 0.0f, 0.0f, 0.0f };

	float dot_product = XMVectorGetX(XMVector3Dot(front, dir));
	float angle = XMVectorGetX(XMVector3AngleBetweenVectors(front, dir));
	if (XMVectorGetX(XMVector3Dot(right, dir)) < 0)
		angle = XM_2PI - angle;

	rotation_ = XMMatrixRotationY(angle);

	movement_component_->accelaration_vector[2] = 1;
}

void GeneralZombie::SetBehaviorTree(const vector<XMVECTOR>& target_poses)
{
	SetPos(target_poses[0] + XMVECTOR{ 0, 50.0f, 0, 0 });

	// setting behavior tree

	// in combat zone
	shared_ptr<SelectorNode> follow_player_or_car = make_shared<SelectorNode>();
	follow_player_or_car->AddChild<EnemyFollowPlayer>(entity_id_, XMVectorZero());
	follow_player_or_car->AddChild<EnemyFollowCar>(entity_id_, XMVectorZero());	

	shared_ptr<InfiniteRepeatNode> repeat_node = make_shared<InfiniteRepeatNode>(follow_player_or_car);

	// to combat zone
	shared_ptr<SequenceNode> move_to_combat_zone = make_shared<SequenceNode>();
	for (auto target_pos : target_poses) {
		move_to_combat_zone->AddChild<EnemyMoveToTarget>(entity_id_, target_pos);
	}

	// root node
	vector<pair<std::function<bool()>, shared_ptr<BehaviorNode>>> children_;
	children_.push_back(make_pair([this]() { return in_defense_bound_; }, repeat_node));
	behavior_tree_.SetRootNode<IfElseIfNode>(children_, move_to_combat_zone);
}

void GeneralZombie::SetMeshId(const string& mesh_id)
{
	C_SkeletalMesh* skm = reg_scene_->try_get< reality::C_SkeletalMesh>(entity_id_);
	skm->skeletal_mesh_id = mesh_id;
}

void GeneralZombie::ChasePlayer()
{
	if (in_defense_bound_ == false)
		return;

	auto player = SCENE_MGR->GetPlayer<Player>(0);
	if (player == nullptr)
		return;

	auto c_enemy_capsule = reg_scene_->try_get<C_CapsuleCollision>(entity_id_);
	if (c_enemy_capsule == nullptr)
		return;

	auto c_player_capsule = reg_scene_->try_get<C_CapsuleCollision>(player->entity_id_);
	if (c_player_capsule == nullptr)
		return;

	RayShape sight_ray;
	sight_ray.start = _XMFLOAT3(GetTipBaseAB(c_enemy_capsule->capsule)[3]);
	sight_ray.end = _XMFLOAT3(GetTipBaseAB(c_player_capsule->capsule)[3]);

	auto callback = QUADTREE->RaycastCarOnly(sight_ray);
	if (callback.success)
		player_in_sight_ = false;
	else
		player_in_sight_ = true;
}

float GeneralZombie::GetCurHp() const
{
	return cur_hp_;
}