#include "BossZombie.h"
#include "AttackEvent.h"
#include "AnimationStateMachine.h"
#include "Player.h"
#include "BossZombie_ASM.h"
#include "BaseEnemy_BTN.h"
#include "BossZombie_BTN.h"

using namespace reality;

void BossZombie::OnInit(entt::registry& registry)
{
	Character::OnInit(registry);
	tag = "boss enemy";

	// setting character data
	AddStatus("hp", CharacterStatus(1000, 1000, 0, 1000));

	AddStatus("car_damage", CharacterStatus(10, 10, 10, 10));
	AddStatus("hook_damage", CharacterStatus(15, 15, 15, 15));
	AddStatus("kick_damage", CharacterStatus(30, 30, 30, 30));
	AddStatus("punch_damage", CharacterStatus(50, 50, 50, 50));
	AddStatus("jump_damage", CharacterStatus(50, 50, 50, 50));

	AddStatus("max_speed", CharacterStatus(RandomIntInRange(200, 250), 0, 200, 250));

	GetMovementComponent()->speed = 0;
	GetMovementComponent()->acceleration = 100;
	GetMovementComponent()->max_speed = GetStatus("max_speed")->GetCurrentValue();

	// set trigger sensor
	C_TriggerSensor trigger_sensor;
	trigger_sensor.can_sense_tags.insert("defense");
	registry.emplace<reality::C_TriggerSensor>(entity_id_, trigger_sensor);

	// setting character objects
	reality::C_SkeletalMesh skm;
	skm.local = TransformS(XMFLOAT3(0.06f, 0.06f, 0.06f)) * XMMatrixRotationY(XMConvertToRadians(180));
	skm.world = skm.local * XMMatrixIdentity();
	skm.skeletal_mesh_id = "DND_BossZombie.skmesh";
	skm.vertex_shader_id = "SkinningVS.cso";
	registry.emplace_or_replace<reality::C_SkeletalMesh>(entity_id_, skm);

	reality::C_CapsuleCollision capsule;
	capsule.tag = "enemy";
	capsule.SetCapsuleData(XMFLOAT3(0, 0, 0), 100, 40);
	registry.emplace<reality::C_CapsuleCollision>(entity_id_, capsule);

	C_SoundGenerator sound_generator;
	sound_generator.local = XMMatrixIdentity();
	sound_generator.world = XMMatrixIdentity();
	registry.emplace<reality::C_SoundGenerator>(entity_id_, sound_generator);

	SkeletalMesh* skeletal_mesh = RESOURCE->UseResource<SkeletalMesh>(skm.skeletal_mesh_id);

	C_Animation animation_component(skeletal_mesh->skeleton.id_bone_map.size());
	animation_component.SetBaseAnimObject<BossZombieBaseAnimationStateMachine>(entity_id_, skm.skeletal_mesh_id, 0);
	reg_scene_->emplace_or_replace<reality::C_Animation>(entity_id_, animation_component);

	transform_tree_.root_node = make_shared<TransformTreeNode>(TYPE_ID(reality::C_CapsuleCollision));
	transform_tree_.AddNodeToNode(TYPE_ID(C_CapsuleCollision), TYPE_ID(C_SkeletalMesh));
	transform_tree_.root_node->OnUpdate(registry, entity_id_, XMMatrixTranslationFromVector(cur_position_));
}

void BossZombie::SetBehaviorTree(const vector<XMVECTOR>& target_poses)
{
	SetPos(target_poses[0] + XMVECTOR{ 0, 50.0f, 0, 0 });

	// in combat zone
	shared_ptr<RandomSelectorNode> boss_zombie_attack_select = make_shared<RandomSelectorNode>();
	boss_zombie_attack_select->AddChild<BossLeftRightHook>(entity_id_);
	boss_zombie_attack_select->SetNumToExecute(3);
	boss_zombie_attack_select->AddChild<BossKickAttack>(entity_id_);
	boss_zombie_attack_select->SetNumToExecute(3);
	boss_zombie_attack_select->AddChild<BossPunchAttack>(entity_id_);
	boss_zombie_attack_select->SetNumToExecute(3);
	boss_zombie_attack_select->AddChild<BossJumpAttack>(entity_id_);
	boss_zombie_attack_select->SetNumToExecute(3);

	vector<shared_ptr<BehaviorNode>> children_sequence;
	children_sequence.push_back(make_shared<BossZombieFollowPlayer>(entity_id_));
	children_sequence.push_back(boss_zombie_attack_select);
	shared_ptr<SequenceNode> boss_zombie_follow_and_attack_player = make_shared<SequenceNode>(children_sequence);

	vector<shared_ptr<BehaviorNode>> children_selector;
	children_selector.push_back(boss_zombie_follow_and_attack_player);
	children_selector.push_back(make_shared<EnemyFollowCar>(entity_id_, XMVectorZero()));
	shared_ptr<SelectorNode> follow_player_or_car = make_shared<SelectorNode>(children_selector);

	shared_ptr<InfiniteRepeatNode> repeat_node = make_shared<InfiniteRepeatNode>(follow_player_or_car);

	// to combat zone
	shared_ptr<SequenceNode> move_to_combat_zone = make_shared<SequenceNode>();
	for (auto target_pos : target_poses) {
		move_to_combat_zone->AddChild<EnemyMoveToTarget>(entity_id_, target_pos);
	}

	// root node
	vector<pair<std::function<bool()>, shared_ptr<BehaviorNode>>> children_root;
	children_root.push_back(make_pair([this]() { return in_defense_bound_; }, repeat_node));
	behavior_tree_.SetRootNode<IfElseIfNode>(children_root, move_to_combat_zone);
}

void BossZombie::LeftHook()
{
	SphereShape attack_sphere = CreateFrontAttackSphere(30.f);

	EVENT->PushEvent<AttackEvent_BoundSphere>(GetStatus("hook_damage")->GetCurrentValue(), attack_sphere, entity_id_);
	EVENT->PushEvent<AttackEvent_AboutCar>(entity_id_);
}

void BossZombie::RightHook()
{
	SphereShape attack_sphere = CreateFrontAttackSphere(30.f);

	EVENT->PushEvent<AttackEvent_BoundSphere>(GetStatus("hook_damage")->GetCurrentValue(), attack_sphere, entity_id_);
	EVENT->PushEvent<AttackEvent_AboutCar>(entity_id_);
}

void BossZombie::KickAttack()
{
	SphereShape attack_sphere = CreateFrontAttackSphere(30.0f);

	EVENT->PushEvent<AttackEvent_BoundSphere>(GetStatus("kick_damage")->GetCurrentValue(), attack_sphere, entity_id_);
	EVENT->PushEvent<AttackEvent_AboutCar>(entity_id_);
}

void BossZombie::PunchAttack()
{	
	SphereShape attack_sphere = CreateFrontAttackSphere(50.0f);
	
	EVENT->PushEvent<AttackEvent_BoundSphere>(GetStatus("punch_damage")->GetCurrentValue(), attack_sphere, entity_id_);
	EVENT->PushEvent<AttackEvent_AboutCar>(entity_id_);
}

void BossZombie::JumpAttack()
{
	EVENT->PushEvent<CameraShakeEvent>(SCENE_MGR->GetPlayer<Player>(0)->entity_id_, 0.3f, 10.0f, 0.2f);

	SphereShape attack_sphere;
	attack_sphere.center = _XMFLOAT3(GetCurPosition());
	attack_sphere.radius = 120.0f;

	EVENT->PushEvent<AttackEvent_BoundSphere>(GetStatus("jump_damage")->GetCurrentValue(), attack_sphere, entity_id_);
	EVENT->PushEvent<AttackEvent_AboutCar>(entity_id_);
}

void BossZombie::Attack()
{
	if (is_attacking_ == false)
	{
		is_attacking_ = true;
		
		cur_attack_type_ = (BossZombieAttackType)RandomIntInRange(0, 3);
	}
	if (is_attack_ended_ == true) {
		is_attacking_ = false;
		is_attack_ended_ = false;
	}
}

SphereShape BossZombie::CreateFrontAttackSphere(float radius)
{
	SphereShape attack_sphere;

	auto c_capsule = GetCapsuleComponent();
	if (c_capsule)
	{
		XMVECTOR point_a = GetTipBaseAB(c_capsule->capsule)[2];
		attack_sphere.center = _XMFLOAT3((point_a + GetFront() * c_capsule->capsule.radius * 2));
		attack_sphere.radius = radius;
	}

	return attack_sphere;
}