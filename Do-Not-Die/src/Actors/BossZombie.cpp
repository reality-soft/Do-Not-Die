#include "BossZombie.h"

void BossZombie::OnInit(entt::registry& registry)
{
	Character::OnInit(registry);
	tag = "enemy";

	// setting character data
	GetMovementComponent()->max_speed = 100;
	GetMovementComponent()->acceleration = 100;
	max_hp_ = cur_hp_ = 100;
	damage_ = 10.0f;

	// set trigger sensor
	C_TriggerSensor trigger_sensor;
	trigger_sensor.can_sense_tags.insert("defense");
	registry.emplace<reality::C_TriggerSensor>(entity_id_, trigger_sensor);

	// setting character objects
	reality::C_SkeletalMesh skm;
	skm.local = TransformS(XMFLOAT3(0.1f, 0.1f, 0.1f));
	skm.world = skm.local * XMMatrixIdentity();
	skm.skeletal_mesh_id = "DND_BossZombie.skmesh";
	skm.vertex_shader_id = "SkinningVS.cso";
	registry.emplace_or_replace<reality::C_SkeletalMesh>(entity_id_, skm);

	reality::C_CapsuleCollision capsule;
	capsule.tag = "enemy";
	capsule.SetCapsuleData(XMFLOAT3(0, 0, 0), 50, 10);
	registry.emplace<reality::C_CapsuleCollision>(entity_id_, capsule);

	SkeletalMesh* skeletal_mesh = RESOURCE->UseResource<SkeletalMesh>(skm.skeletal_mesh_id);
	if (skeletal_mesh)
	{
		C_Animation animation_component(skeletal_mesh->skeleton.id_bone_map.size());

		animation_component.SetBaseAnimObject<AnimationBase>(skm.skeletal_mesh_id, 0);

		// TEST _ BOSS ZOMBIE
		//animation_component.GetAnimSlotByName("Base")->SetAnimation("DND_BossZombie_Idle.anim", 0.0);
		//animation_component.GetAnimSlotByName("Base")->SetAnimation("DND_BossZombie_JumpAttack.anim", 0.0);
		//animation_component.GetAnimSlotByName("Base")->SetAnimation("DND_BossZombie_Kick.anim", 0.0);
		//animation_component.GetAnimSlotByName("Base")->SetAnimation("DND_BossZombie_Punch_L.anim", 0.0);
		//animation_component.GetAnimSlotByName("Base")->SetAnimation("DND_BossZombie_Punch_R.anim", 0.0);
		//animation_component.GetAnimSlotByName("Base")->SetAnimation("DND_BossZombie_Run.anim", 0.0);
		//animation_component.GetAnimSlotByName("Base")->SetAnimation("DND_BossZombie_StrongPunch.anim", 0.0);
		animation_component.GetAnimSlotByName("Base")->SetAnimation("DND_BossZombie_Walk.anim", 0.0);
		reg_scene_->emplace_or_replace<reality::C_Animation>(entity_id_, animation_component);
	}

	transform_tree_.root_node = make_shared<TransformTreeNode>(TYPE_ID(reality::C_CapsuleCollision));
	transform_tree_.AddNodeToNode(TYPE_ID(C_CapsuleCollision), TYPE_ID(C_SkeletalMesh));
	transform_tree_.root_node->OnUpdate(registry, entity_id_, XMMatrixTranslationFromVector(cur_position_));
}


void BossZombie::OnUpdate()
{

}