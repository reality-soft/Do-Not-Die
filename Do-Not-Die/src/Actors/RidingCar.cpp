#include "RidingCar.h"

void RidingCar::OnInit(entt::registry& registry)
{
	Actor::OnInit(registry);
	tag = "riding_car";

	ride_track.Import("DND_CarRiding_1.mapdat", GuideType::eNpcTrack);

	reality::C_SkeletalMesh skm;
	skm.skeletal_mesh_id = "DND_Car.skmesh";
	skm.vertex_shader_id = "SkinningVS.cso";
	skm.local = TransformS(XMFLOAT3(0.8, 0.8, 0.8)) * TransformR(XMFLOAT3(0, 0, 0)) * TransformT(XMFLOAT3(0, -15.0f, 0));
	skm.world = skm.local * XMMatrixIdentity();
	registry.emplace_or_replace<reality::C_SkeletalMesh>(entity_id_, skm);

	auto skeletal_mesh = RESOURCE->UseResource<SkeletalMesh>(skm.skeletal_mesh_id);

	C_Animation animation_component(skeletal_mesh->skeleton.id_bone_map.size());
	animation_component.SetBaseAnimObject<AnimationBase>(skm.skeletal_mesh_id, 0);
	animation_component.GetAnimSlotByName("Base")->SetAnimation("DND_Car_Riding_Anim.anim", 0.0);
	reg_scene_->emplace_or_replace<reality::C_Animation>(entity_id_, animation_component);

	C_SoundGenerator sound_generator;
	registry.emplace<C_SoundGenerator>(entity_id_, sound_generator);

	transform_tree_.root_node = make_shared<TransformTreeNode>(TYPE_ID(C_SkeletalMesh));
	transform_tree_.root_node->OnUpdate(registry, entity_id_, XMMatrixTranslationFromVector(ride_track.line_nodes.at(0)));
}

void RidingCar::OnUpdate()
{
	float ride_speed = 300.0f;
	XMVECTOR ride_vector = XMVector3Normalize(ride_track.line_nodes.at(1) - ride_track.line_nodes.at(0));
	ride_vector *= ride_speed * TM_DELTATIME;	
	ApplyMovement(GetCurPosition() + ride_vector);
}