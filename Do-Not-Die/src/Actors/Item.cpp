#include "Engine_include.h"
#include "Item.h"

Item::Item(ItemType item_type, XMFLOAT3 spawn_point, float trigger_radius)
{
	trigger_sphere_.center = spawn_point;
	trigger_sphere_.radius = trigger_radius;
	item_type_ = item_type;
}

void Item::OnInit(entt::registry& registry)
{
	Actor::OnInit(registry);
	visible = true;

	reality::C_StaticMesh stm;
	stm.local = XMMatrixIdentity();
	stm.world = XMMatrixIdentity();
	stm.vertex_shader_id = "StaticMeshVS.cso";

	switch (item_type_)
	{
	case ItemType::eHealKit:
		stm.static_mesh_id = "HealthKit.stmesh";
		break;
	case ItemType::eMedicalBox:
		stm.static_mesh_id = "MedicalBox.stmesh";
		break;
	case ItemType::eEnergyDrink:
		stm.static_mesh_id = "EnergyDrink.stmesh";
		break;
	case ItemType::eDrug:
		stm.static_mesh_id = "Drug.stmesh";
		break;
	case ItemType::eAR_Ammo:
		stm.static_mesh_id = "AR_Ammo.stmesh";
		break;
	case ItemType::ePistol_Ammo:
		stm.static_mesh_id = "Pistol_Ammo.stmesh";
		break;
	case ItemType::eGrenade:
		stm.static_mesh_id = "Grenade.stmesh";
		break;
	case ItemType::eRepairPart:
		stm.static_mesh_id = "RepairPart.stmesh";
		break;
	}

	registry.emplace_or_replace<reality::C_StaticMesh>(entity_id_, stm);

	reality::C_TriggerVolume item_trigger;
	item_trigger.sphere_volume = trigger_sphere_;
	item_trigger.tag = "item";
	registry.emplace_or_replace<reality::C_TriggerVolume>(entity_id_, item_trigger);

	transform_tree_.root_node = make_shared<reality::TransformTreeNode>(TYPE_ID(reality::C_StaticMesh));
	transform_matrix_ = TransformT(trigger_sphere_.center);
	transform_tree_.root_node->OnUpdate(registry, entity_id_, transform_matrix_);
}

void Item::OnUpdate()
{
}
