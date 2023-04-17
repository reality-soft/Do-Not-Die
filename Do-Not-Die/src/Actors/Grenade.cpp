#include "Grenade.h"
#include "Engine_include.h"
using namespace reality;

void Grenade::OnInit(entt::registry& registry)
{
	Actor::OnInit(registry);

	reality::C_SphereCollision sphere_comp;
	sphere_comp.SetSphereData(XMFLOAT3(0, 0, 0), 5);
	registry.emplace_or_replace<reality::C_SphereCollision>(entity_id_, sphere_comp);

	reality::C_StaticMesh stm;
	stm.local = XMMatrixIdentity();
	stm.world = XMMatrixIdentity();
	stm.static_mesh_id = "grenade.stmesh";
	stm.vertex_shader_id = "StaticMeshVS.cso";
	registry.emplace_or_replace<reality::C_StaticMesh>(entity_id_, stm);

	QUADTREE->RegistDynamicSphere(GetEntityId());

	transform_tree_.root_node = make_shared<TransformTreeNode>(TYPE_ID(C_SphereCollision));
	transform_tree_.AddNodeToNode(TYPE_ID(C_SphereCollision), TYPE_ID(C_StaticMesh));

	transform_tree_.root_node->OnUpdate(registry, entity_id_, transform_matrix_);
}

void Grenade::OnUpdate()
{
	XMFLOAT3 gravity = { 0.0f, -9.8f, 0.0f };
	dir_.y += gravity.y * TIMER->GetDeltaTime();

	XMVECTOR s, r, t;
	XMMatrixDecompose(&s, &r, &t, transform_matrix_);
	XMVECTOR current_pos = XMVectorAdd(t, XMLoadFloat3(&dir_));

	transform_matrix_ = XMMatrixTranslationFromVector(current_pos);
	transform_tree_.root_node->OnUpdate(*reg_scene_, entity_id_, transform_matrix_);

	auto& sphere_comp = reg_scene_->get<C_SphereCollision>(GetEntityId());
	if (sphere_comp.is_collide)
	{
		int i = 0;
	}
}


void Grenade::SetPos(FXMVECTOR pos)
{
	transform_matrix_ = XMMatrixTranslationFromVector(pos);
	transform_tree_.root_node->OnUpdate(*reg_scene_, entity_id_, transform_matrix_);
}


void Grenade::SetDir(FXMVECTOR dir, float speed)
{
	XMVECTOR new_dir = XMVectorScale(dir, speed);
	XMStoreFloat3(&dir_, new_dir);
}
