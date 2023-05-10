#include "NormalZombie.h"
#include "AttackEvent.h"
#include "AnimationStateMachine.h"
#include "Player.h"
#include "NormalZombie_ASM.h"
#include "BaseEnemy_BTN.h"
#include "NormalZombie_BTN.h"

using namespace reality;

void NormalZombie::OnInit(entt::registry& registry)
{
	Character::OnInit(registry);
	tag = "enemy";

	//// setting character data
	//GetMovementComponent()->max_speed = 100;
	//GetMovementComponent()->acceleration = 100;
	//max_hp_ = cur_hp_ = 100;
	//damage_ = 10.0f;

	AddStatus("hp", CharacterStatus(100, 100, 0, 100));
	AddStatus("default_damage", CharacterStatus(10, 10, 10, 10));
	AddStatus("max_speed", CharacterStatus(RandomIntInRange(100, 200), 0, 100, 200));

	GetMovementComponent()->speed = 0;
	GetMovementComponent()->acceleration = 100;
	GetMovementComponent()->max_speed = GetStatus("max_speed")->GetCurrentValue();


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

	C_SoundGenerator sound_generator;
	sound_generator.local = XMMatrixIdentity();
	sound_generator.world = XMMatrixIdentity();
	registry.emplace<reality::C_SoundGenerator>(entity_id_, sound_generator);

	transform_tree_.root_node = make_shared<TransformTreeNode>(TYPE_ID(reality::C_CapsuleCollision));
	transform_tree_.AddNodeToNode(TYPE_ID(C_CapsuleCollision), TYPE_ID(C_SkeletalMesh));
	transform_tree_.AddNodeToNode(TYPE_ID(C_CapsuleCollision), TYPE_ID(C_SoundGenerator));

	reality::C_SkeletalMesh* skm_ptr = registry.try_get<C_SkeletalMesh>(entity_id_);
	skm_ptr->local = XMMatrixScalingFromVector({ 0.3, 0.3, 0.3, 0.0 }) * XMMatrixRotationY(XMConvertToRadians(180.f));

	cur_position_ = { 0.f, 100.f, 0.f, 0.f };
	transform_tree_.root_node->OnUpdate(registry, entity_id_, XMMatrixTranslationFromVector(cur_position_));


	SkeletalMesh* skeletal_mesh = RESOURCE->UseResource<SkeletalMesh>(skm.skeletal_mesh_id);
	C_Animation animation_component(skeletal_mesh->skeleton.id_bone_map.size());
	animation_component.SetBaseAnimObject<NormalZombieBaseAnimationStateMachine>(entity_id_, skm.skeletal_mesh_id, 0);
	animation_component.AddNewAnimSlot<NormalZombieUpperBodyAnimationStateMachine>("UpperBody", entity_id_, skm.skeletal_mesh_id, 3, "Spine_02");
	reg_scene_->emplace_or_replace<reality::C_Animation>(entity_id_, animation_component);
}

void NormalZombie::Attack()
{
	if (is_attacking_ == true)
		return;

	auto c_enemy_capsule = reg_scene_->try_get<C_CapsuleCollision>(entity_id_);
	if (c_enemy_capsule == nullptr)
		return;

	is_attacking_ = true;
	 
	//RayShape attack_ray;
	//attack_ray.start = _XMFLOAT3(GetTipBaseAB(c_enemy_capsule->capsule)[3]);
	//attack_ray.end = _XMFLOAT3((_XMVECTOR3(attack_ray.start) + (front_ * attack_distance_)));
	//EVENT->PushEvent<AttackEvent_SingleRay>(attack_ray, entity_id_);
}

void NormalZombie::AddImpulse(XMVECTOR direction, float strength)
{
	if (GetStatus("hp")->GetCurrentValue() <= 0)
		return;	

	CancelMovement();
	auto c_capsule = GetCapsuleComponent();
	if (c_capsule)
	{
		c_capsule->impulse_vector = direction;
		c_capsule->impulse_strength = strength;	
		GetMovementComponent()->gravity_pulse = 0;
	}
}

void NormalZombie::SetBehaviorTree(const vector<XMVECTOR>& target_poses)
{
	SetPos(target_poses[0] + XMVECTOR{ 0, 50.0f, 0, 0 });

	// setting behavior tree

	// in combat zone
	shared_ptr<SelectorNode> follow_player_or_car = make_shared<SelectorNode>();
	follow_player_or_car->AddChild<NormalZombieFollowPlayer>(entity_id_, XMVectorZero());
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

void NormalZombie::SetMeshId(const string& mesh_id)
{
	C_SkeletalMesh* skm = reg_scene_->try_get< reality::C_SkeletalMesh>(entity_id_);
	skm->skeletal_mesh_id = mesh_id;
}