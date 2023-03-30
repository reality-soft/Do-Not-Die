#include "stdafx.h"
#include "TestPointLightActor.h"
#include "SceneMgr.h"
#include "ResourceMgr.h"

using namespace reality;

void TestPointLightActor::OnInit(entt::registry& registry)
{
	PointLightActor::OnInit(registry);

	auto& pointlight_comp = registry.get<C_PointLight>(GetEntityId());
	pointlight_comp.light_color = { 1.0f, 1.0f, 1.0f, 1.0f };
	pointlight_comp.lifetime = -1.0f;
	pointlight_comp.timer = 0.0f;
	pointlight_comp.attenuation = { 2.0f, 0.0f, 0.0f };
	pointlight_comp.range = 100.0f;
	pointlight_comp.position = { 100.0f, 10.0f, 100.0f };
}

void TestPointLightActor::OnUpdate()
{
	PointLightActor::OnUpdate();
}
