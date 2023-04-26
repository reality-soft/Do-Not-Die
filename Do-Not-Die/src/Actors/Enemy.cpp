#include "Enemy.h"
#include "AnimationStateMachine.h"
#include "Player.h"
#include "Enemy_ASM.h"
#include "Enemy_BTN.h"

using namespace reality;

void Enemy::OnInit(entt::registry& registry)
{
	Character::OnInit(registry);

	// setting character data
	movement_component_->max_speed = 60;
	movement_component_->acceleration = 60;
	max_hp_ = cur_hp_ = 100;

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
	capsule.tag = "Enemy";
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
	animation_component.SetBaseAnimObject<ZombieAnimationStateMachine>(entity_id_, skm.skeletal_mesh_id, 0);
	reg_scene_->emplace_or_replace<reality::C_Animation>(entity_id_, animation_component);
	SetCharacterAnimation("Zombie_Idle_1_v2_IPC_Anim_Unreal Take.anim");
}

void Enemy::OnUpdate()
{
	behavior_tree_.Update();
	Character::OnUpdate();
	ChasePlayer();
}

void Enemy::SetCharacterAnimation(string anim_id) const
{
	reality::C_Animation* animation_component_ptr = reg_scene_->try_get<reality::C_Animation>(entity_id_);
	int base_index = animation_component_ptr->name_to_anim_slot_index["Base"];
	animation_component_ptr->anim_slots[base_index].second->SetAnimation(anim_id, 0.3);
	reg_scene_->emplace_or_replace<reality::C_Animation>(entity_id_, *animation_component_ptr);
}

void Enemy::Jump()
{
}

void Enemy::Idle()
{
}

void Enemy::Attack()
{
	is_attacking_ = true;
}

float Enemy::GetMaxHp() const
{
	return max_hp_;
}

void Enemy::SetCurHp(int hp)
{
}

void Enemy::TakeDamage(int damage)
{
	if (is_hit_ == false) {
		is_hit_ = true;
		cur_hp_ -= damage;
	}
}

void Enemy::SetMovement(const XMVECTOR& direction)
{
	if (XMVector3Length(direction).m128_f32[0] <= 0.00001f) {
		return;
	}

	is_moving_ = true;
	XMVECTOR front = { 0.0f, 0.0f, 1.0f, 0.0f };
	XMVECTOR right = { 1.0f, 0.0f, 0.0f, 0.0f };

	float dot_product = XMVectorGetX(XMVector3Dot(front, direction));

	float angle = XMVectorGetX(XMVector3AngleBetweenNormals(front, direction));

	if (XMVectorGetX(XMVector3Dot(right, direction)) < 0)
		angle = XM_2PI - angle;

	angle = XMConvertToDegrees(angle);

	rotation_ = XMMatrixRotationY(angle);

	movement_component_->accelaration_vector[2] = 1;
}

void Enemy::SetBehaviorTree(const vector<XMVECTOR>& target_poses)
{
	SetPos(target_poses[0] + XMVECTOR{ 0, 50.0f, 0, 0 });

	// setting behavior tree

	// in combat zone
	shared_ptr<SelectorNode> follow_player_or_car = make_shared<SelectorNode>();
	shared_ptr<SequenceNode> follow_and_attack_player = make_shared<SequenceNode>();
	follow_and_attack_player->AddChild<EnemyFollowPlayer>(entity_id_, SCENE_MGR->GetPlayer<Player>(0)->GetCurPosition());
	follow_and_attack_player->AddChild<EnemyAttack>(entity_id_);
	follow_player_or_car->AddChild<SequenceNode>(*follow_and_attack_player);
	
	shared_ptr<SequenceNode> follow_and_attack_car = make_shared<SequenceNode>();
	follow_and_attack_car->AddChild<EnemyFollowCar>(entity_id_, XMVectorZero());
	follow_and_attack_car->AddChild<EnemyAttack>(entity_id_);
	follow_player_or_car->AddChild<SequenceNode>(*follow_and_attack_car);

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

void Enemy::SetMeshId(const string& mesh_id)
{
	C_SkeletalMesh* skm = reg_scene_->try_get< reality::C_SkeletalMesh>(entity_id_);
	skm->skeletal_mesh_id = mesh_id;
}

void Enemy::ChasePlayer()
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

	UINT casted_triangle = 0;
	const auto& triangles = QUADTREE->GetCarTriangles();
	for (const auto& tri : triangles)
	{
		auto callback = RayToTriangle(sight_ray, tri);
		if (callback.success)
			casted_triangle++;
	}
	if (casted_triangle == 0)
		player_in_sight_ = true;
	else
		player_in_sight_ = false;
}

float Enemy::GetCurHp() const
{
	return cur_hp_;
}
