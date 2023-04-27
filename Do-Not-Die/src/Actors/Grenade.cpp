#include "Grenade.h"
#include "Engine_include.h"
#include "FX_Explosion.h"
#include "EventMgr.h"
#include "Player.h"

using namespace reality;

void Grenade::OnInit(entt::registry& registry)
{
	Actor::OnInit(registry);

	reality::C_SphereCollision sphere_comp;
	sphere_comp.SetSphereData(XMFLOAT3(0, 0, 0), 3);
	registry.emplace_or_replace<reality::C_SphereCollision>(entity_id_, sphere_comp);

	reality::C_StaticMesh stm;
	stm.local = XMMatrixIdentity();
	stm.world = XMMatrixIdentity();
	stm.static_mesh_id = "Grenade.stmesh";
	stm.vertex_shader_id = "StaticMeshVS.cso";
	registry.emplace_or_replace<reality::C_StaticMesh>(entity_id_, stm);

	QUADTREE->RegistDynamicSphere(GetEntityId());

	transform_tree_.root_node = make_shared<TransformTreeNode>(TYPE_ID(C_SphereCollision));
	transform_tree_.AddNodeToNode(TYPE_ID(C_SphereCollision), TYPE_ID(C_StaticMesh));

	timer_ = 0.0f;
	explosion_time_ = 3.0f;
	range_ = 100.0f;
	damage_ = 300.0f;
}

void Grenade::OnUpdate()
{
	timer_ += TIMER->GetDeltaTime();
	if (explosion_time_ < timer_ && !exploded_)
	{
		exploded_ = true;
		EFFECT_MGR->SpawnEffect<FX_Explosion>(cur_position_);

		auto player = SCENE_MGR->GetPlayer<Player>(0);
		player->GetCurPosition();
		EVENT->PushEvent<CameraShakeEvent>(player->GetEntityId(), 0.3f, 10.0f, 0.2f);
		EVENT->PushEvent<DeleteActorEvent>(GetEntityId());
	}
	else 
	{
		XMFLOAT3 gravity = { 0.0f, -9.8f, 0.0f };
		float speed = XMVectorGetX(XMVector3Length(XMLoadFloat3(&dir_)));
		if (speed > 0.2f)
		{
			dir_.y += gravity.y * TIMER->GetDeltaTime();
			cur_position_ = XMVectorAdd(cur_position_, XMLoadFloat3(&dir_));

			transform_tree_.root_node->OnUpdate(*reg_scene_, entity_id_, XMMatrixTranslationFromVector(cur_position_));
		}

		auto& sphere_comp = reg_scene_->get<C_SphereCollision>(GetEntityId());

		if (sphere_comp.is_collide && !exploded_)
		{
			XMVECTOR dir = XMLoadFloat3(&dir_);
			XMVECTOR normal = XMVector3Normalize(XMLoadFloat3(&sphere_comp.tri_normal));
			XMVECTOR reflect = (dir - 2.0 * XMVector3Dot(dir, normal) * normal) * 0.7f;
			float dot = XMVectorGetX(XMVector3Dot(dir, normal));
			if (dot < 0)
				XMStoreFloat3(&dir_, reflect);
		}
	}
	
}


void Grenade::SetPos(FXMVECTOR pos)
{
	cur_position_ = pos;
	transform_tree_.root_node->OnUpdate(*reg_scene_, entity_id_, XMMatrixTranslationFromVector(cur_position_));
}


void Grenade::SetDir(FXMVECTOR dir, float speed)
{
	XMVECTOR new_dir = XMVectorScale(dir, speed);
	XMStoreFloat3(&dir_, new_dir);
}
