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
	tag = "enemy";

	// setting character data
	GetMovementComponent()->max_speed = 100;
	GetMovementComponent()->acceleration = 100;

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
	skm.local = TransformS(XMFLOAT3(0.06f, 0.06f, 0.06f)) * XMMatrixRotationY(XMConvertToRadians(180));
	skm.world = skm.local * XMMatrixIdentity();
	skm.skeletal_mesh_id = "DND_BossZombie.skmesh";
	skm.vertex_shader_id = "SkinningVS.cso";
	registry.emplace_or_replace<reality::C_SkeletalMesh>(entity_id_, skm);

	reality::C_CapsuleCollision capsule;
	capsule.tag = "enemy";
	capsule.SetCapsuleData(XMFLOAT3(0, 0, 0), 100, 40);
	registry.emplace<reality::C_CapsuleCollision>(entity_id_, capsule);

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
	boss_zombie_attack_select->AddChild<BossPunchAttack>(entity_id_);
	boss_zombie_attack_select->SetNumToExecute(3);
	boss_zombie_attack_select->AddChild<BossJumpAttack>(entity_id_);
	boss_zombie_attack_select->SetNumToExecute(3);

	vector<pair<std::function<bool()>, shared_ptr<BehaviorNode>>> children_if_else;
	children_if_else.push_back(
		make_pair([this]() {
		Player* player = reality::SCENE_MGR->GetPlayer<Player>(0);
		BaseEnemy* enemy = reality::SCENE_MGR->GetActor<BaseEnemy>(entity_id_);
		float distance_to_player = Distance(player->GetCurPosition(), enemy->GetCurPosition());
		return (distance_to_player >= 100); 
			}, 
		make_shared<BossZombieFollowPlayer>(entity_id_)));
	shared_ptr<IfElseIfNode> boss_zombie_follow_and_attack_player = make_shared<IfElseIfNode>(children_if_else, dynamic_pointer_cast<BehaviorNode>(boss_zombie_attack_select));

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

void BossZombie::PunchAttack()
{
	if (is_attacking_ == true)
		return;

	is_attacking_ = true;
	cur_attack_type_ = BossZombieAttackType::PUNCH_ATTACK;

	auto c_enemy_capsule = reg_scene_->try_get<C_CapsuleCollision>(entity_id_);
	if (c_enemy_capsule == nullptr)
		return;

	SphereShape attack_sphere;
	auto capsule_info = GetTipBaseAB(c_enemy_capsule->capsule);
	XMVECTOR shepre_center = capsule_info[3] + (front_ * c_enemy_capsule->capsule.radius * 3);
	attack_sphere.center = _XMFLOAT3(shepre_center);
	attack_sphere.radius = c_enemy_capsule->capsule.radius * 2;
	EVENT->PushEvent<AttackEvent_BoundSphere>(20.0f, attack_sphere, entity_id_);
}

void BossZombie::JumpAttack()
{
	if (is_attacking_ == true)
		return;

	is_attacking_ = true;
	cur_attack_type_ = BossZombieAttackType::JUMP_ATTACK;

	auto c_enemy_capsule = reg_scene_->try_get<C_CapsuleCollision>(entity_id_);
	if (c_enemy_capsule == nullptr)
		return;

	SphereShape attack_sphere;
	auto capsule_info = GetTipBaseAB(c_enemy_capsule->capsule);
	XMVECTOR shepre_center = capsule_info[3] + (front_ * c_enemy_capsule->capsule.radius * 3);
	attack_sphere.center = _XMFLOAT3(shepre_center);
	attack_sphere.radius = c_enemy_capsule->capsule.radius * 2;
	EVENT->PushEvent<AttackEvent_BoundSphere>(20.0f, attack_sphere, entity_id_);
}
