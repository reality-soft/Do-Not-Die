#include "stdafx.h"
#include "UI_Minimap.h"
#include "Player.h"
#include "BossZombie.h"
#include "NormalZombie.h"
#include "InGameScene.h"

#define MINIMAP_OFFSET 0.1
#define MINIMAP_LENGTH 15800.0f

using namespace reality;

void UI_Minimap::InitMinimap(string minimap_id)
{
	UIBase::Init();

	reg_ = &SCENE_MGR->GetScene(INGAME)->GetRegistryRef();

	minimap_tex_id_ = minimap_id;
	auto texture = RESOURCE->UseResource<Texture>(minimap_tex_id_);

	minimap_rt = RENDER_TARGET->MakeRT("Minimap UI", texture->texture_desc.Width, texture->texture_desc.Height);
	minimap_rt->SetClearColor({ 0, 0, 0, 0 });

	// Make Vertex Buffer to Make Minimap's Vertex Buffer
	MakeMinimapVB();
}

void UI_Minimap::UpdateThisUI()
{
	UIBase::UpdateThisUI();

	// 0. Set RenderTarget to MinimapRT
	minimap_rt->SetRenderTarget();

	// 1. Render Minimap Image
	RenderMinimap();

	// 2. Render Truck Icon
	auto ingame_scene = (InGameScene*)SCENE_MGR->GetScene(INGAME).get();
	XMMATRIX truck_world = XMMatrixTranslationFromVector(ingame_scene->GetWaveSystem().GetCarPosition());
	RenderTruckIcon(truck_world);

	// 3. Render Actor Icon
	auto view_of_capsule = reg_->view<C_CapsuleCollision>();
	for (auto& entity : view_of_capsule)
	{
		auto actor = SCENE_MGR->GetActor<Actor>(entity);
		
		// if player, continue;
		if (actor->tag == "player")
		{
			auto player = (Player*)actor;
			XMMATRIX player_world = player->GetRotation() * XMMatrixTranslationFromVector(player->GetCurPosition());
			RenderPlayerIcon(player_world);
		}
		else if (actor->tag == "boss enemy")
		{
			// Boss Zombie Icon
			auto boss_zombie = (BossZombie*)actor;
			XMMATRIX boss_world = XMMatrixTranslationFromVector(boss_zombie->GetCurPosition());
			RenderBossIcon(boss_world);
		}
		else if(actor->tag == "enemy")
		{
			// Zombie Icon
			auto zombie = (NormalZombie*)actor;
			if (zombie)
			{
				XMMATRIX zombie_world = XMMatrixTranslationFromVector(zombie->GetCurPosition());
				RenderZombieIcon(zombie_world);
			}
		}
	}

	// 4. Return from Minimap RenderTarget to Backbuffer
	DX11APP->SetBackBufferRTV();
}

void UI_Minimap::RenderThisUI()
{
	UpdateTextureCoordAlongPlayer();

	auto resolution = ENGINE->GetWindowResolution();
	UISystem::SetCbData(XMMatrixTranspose(rect_transform_[resolution].world_matrix));

	// Set Topology
	DX11APP->GetDeviceContext()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Set Vertex Buffer
	UINT stride = sizeof(UIVertex);
	UINT offset = 0;
	DX11APP->GetDeviceContext()->IASetVertexBuffers(0, 1, minimap_vertex_buf_.GetAddressOf(), &stride, &offset);

	// Set Index Buffer
	DX11APP->GetDeviceContext()->IASetIndexBuffer(render_data_.index_buffer.Get(), DXGI_FORMAT_R32_UINT, offset);

	// Get Shader From ResourceMgr
	VertexShader* vs = RESOURCE->UseResource<VertexShader>(render_data_.vs_id);
	PixelShader* ps = RESOURCE->UseResource<PixelShader>(render_data_.ps_id);

	if (vs == nullptr || ps == nullptr)
		return;

	// Set InputLayout
	DX11APP->GetDeviceContext()->IASetInputLayout(vs->InputLayout());

	// Set VS
	DX11APP->GetDeviceContext()->VSSetShader(vs->Get(), 0, 0);

	// Set PS
	DX11APP->GetDeviceContext()->PSSetShader(ps->Get(), 0, 0);

	// Reset GS
	DX11APP->GetDeviceContext()->GSSetShader(nullptr, 0, 0);

	// Set Texture
	DX11APP->GetDeviceContext()->PSSetShaderResources(0, 1, minimap_rt->render_target_view_srv_.GetAddressOf());

	// Draw Indexed
	DX11APP->GetDeviceContext()->DrawIndexed(render_data_.index_list.size(), 0, 0);
}

void UI_Minimap::MakeMinimapVB()
{
	minimap_vertex_list_ = render_data_.vertex_list;

	D3D11_BUFFER_DESC bufDesc;

	ZeroMemory(&bufDesc, sizeof(D3D11_BUFFER_DESC));

	bufDesc.ByteWidth = sizeof(UIVertex) * minimap_vertex_list_.size();
	bufDesc.Usage = D3D11_USAGE_DEFAULT;
	bufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufDesc.CPUAccessFlags = 0;
	bufDesc.MiscFlags = 0;
	bufDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA subResourse;

	ZeroMemory(&subResourse, sizeof(D3D11_SUBRESOURCE_DATA));

	subResourse.pSysMem = &minimap_vertex_list_.at(0);
	subResourse.SysMemPitch;
	subResourse.SysMemSlicePitch;

	DX11APP->GetDevice()->CreateBuffer(&bufDesc, &subResourse, minimap_vertex_buf_.GetAddressOf());
}

void UI_Minimap::RenderMinimap()
{
	// Set Constant Buffer
	XMVECTOR pos = XMVectorSet(0.5f, 0.5f, 0.0f, 0.0f);
	XMMATRIX S, R, T;
	S = XMMatrixIdentity();
	R = XMMatrixIdentity();
	T = XMMatrixTranslationFromVector(pos);
	XMMATRIX minimap_world = S * R * T;

	UISystem::SetCbData(XMMatrixTranspose(minimap_world));

	// Set Texture
	Texture* texture = RESOURCE->UseResource<Texture>(minimap_tex_id_);
	DX11APP->GetDeviceContext()->PSSetShaderResources(0, 1, texture->srv.GetAddressOf());

	RenderExceptTextureSet();
}

void UI_Minimap::RenderPlayerIcon(XMMATRIX world)
{
	// Regulate World Coord
	XMVECTOR world_scale, world_rot, world_pos;
	XMMatrixDecompose(&world_scale, &world_rot, &world_pos, world);

	XMMATRIX S, R, T;

	// Scale
	XMVECTOR scale = XMVectorSet(0.024f, 0.024f, 0.0f, 1.0f);
	S = XMMatrixScalingFromVector(scale);
	// Rotation
	C_Camera& player_camera = reg_->get<C_Camera>(SCENE_MGR->GetPlayer<Character>(0)->GetEntityId());
	R = XMMatrixRotationZ(-player_camera.pitch_yaw.y);
	// Translation
	XMVECTOR pos = ConvertWorldToScreenXZ(world_pos);
	XMStoreFloat2(&minimap_player_pos, pos);
	T = XMMatrixTranslationFromVector(pos);
	XMMATRIX minimap_world = S * R * T;

	// Set Constant Buffer
	UISystem::SetCbData(XMMatrixTranspose(minimap_world));

	// Set Texture
	Texture* texture = RESOURCE->UseResource<Texture>("T_Player_Icon.png");
	DX11APP->GetDeviceContext()->PSSetShaderResources(0, 1, texture->srv.GetAddressOf());

	RenderExceptTextureSet();
}

void UI_Minimap::RenderZombieIcon(XMMATRIX world)
{
	// Regulate World Coord
	XMVECTOR world_scale, world_rot, world_pos;
	XMMatrixDecompose(&world_scale, &world_rot, &world_pos, world);

	XMMATRIX S, R, T;

	// Scale
	XMVECTOR scale = XMVectorSet(0.016f, 0.016f, 0.0f, 1.0f);
	S = XMMatrixScalingFromVector(scale);
	// Rotation
	R = XMMatrixIdentity();
	// Translation
	XMVECTOR pos = ConvertWorldToScreenXZ(world_pos);
	XMStoreFloat2(&minimap_player_pos, pos);
	T = XMMatrixTranslationFromVector(pos);
	XMMATRIX minimap_world = S * R * T;

	// Set Constant Buffer
	UISystem::SetCbData(XMMatrixTranspose(minimap_world));

	// Set Texture
	Texture* texture = RESOURCE->UseResource<Texture>("T_Zombie_Icon.png");
	DX11APP->GetDeviceContext()->PSSetShaderResources(0, 1, texture->srv.GetAddressOf());

	RenderExceptTextureSet();
}

void UI_Minimap::RenderTruckIcon(XMMATRIX world)
{
	// Regulate World Coord
	XMVECTOR world_scale, world_rot, world_pos;
	XMMatrixDecompose(&world_scale, &world_rot, &world_pos, world);

	XMMATRIX S, R, T;

	// Scale
	XMVECTOR scale = XMVectorSet(0.032f, 0.032f, 0.0f, 1.0f);
	S = XMMatrixScalingFromVector(scale);
	// Rotation
	R = XMMatrixIdentity();
	// Translation
	XMVECTOR pos = ConvertWorldToScreenXZ(world_pos);
	XMStoreFloat2(&minimap_player_pos, pos);
	T = XMMatrixTranslationFromVector(pos);
	XMMATRIX minimap_world = S * R * T;

	// Set Constant Buffer
	UISystem::SetCbData(XMMatrixTranspose(minimap_world));

	// Set Texture
	Texture* texture = RESOURCE->UseResource<Texture>("T_TruckIcon.png");
	DX11APP->GetDeviceContext()->PSSetShaderResources(0, 1, texture->srv.GetAddressOf());

	RenderExceptTextureSet();
}

void UI_Minimap::RenderBossIcon(XMMATRIX world)
{
	// Regulate World Coord
	XMVECTOR world_scale, world_rot, world_pos;
	XMMatrixDecompose(&world_scale, &world_rot, &world_pos, world);

	XMMATRIX S, R, T;

	// Scale
	XMVECTOR scale = XMVectorSet(0.024f, 0.024f, 0.0f, 1.0f);
	S = XMMatrixScalingFromVector(scale);
	// Rotation
	R = XMMatrixIdentity();
	// Translation
	XMVECTOR pos = ConvertWorldToScreenXZ(world_pos);
	XMStoreFloat2(&minimap_player_pos, pos);
	T = XMMatrixTranslationFromVector(pos);
	XMMATRIX minimap_world = S * R * T;

	// Set Constant Buffer
	UISystem::SetCbData(XMMatrixTranspose(minimap_world));

	// Set Texture
	Texture* texture = RESOURCE->UseResource<Texture>("T_Boss_Icon.png");
	DX11APP->GetDeviceContext()->PSSetShaderResources(0, 1, texture->srv.GetAddressOf());

	RenderExceptTextureSet();
}

void UI_Minimap::UpdateTextureCoordAlongPlayer()
{
	minimap_vertex_list_[0].t.x = minimap_player_pos.x - MINIMAP_OFFSET;
	minimap_vertex_list_[0].t.y = minimap_player_pos.y - MINIMAP_OFFSET;
	minimap_vertex_list_[1].t.x = minimap_player_pos.x + MINIMAP_OFFSET;
	minimap_vertex_list_[1].t.y = minimap_player_pos.y - MINIMAP_OFFSET;
	minimap_vertex_list_[2].t.x = minimap_player_pos.x - MINIMAP_OFFSET;
	minimap_vertex_list_[2].t.y = minimap_player_pos.y + MINIMAP_OFFSET;
	minimap_vertex_list_[3].t.x = minimap_player_pos.x + MINIMAP_OFFSET;
	minimap_vertex_list_[3].t.y = minimap_player_pos.y + MINIMAP_OFFSET;

	D3D11_SUBRESOURCE_DATA subResourse;

	ZeroMemory(&subResourse, sizeof(D3D11_SUBRESOURCE_DATA));

	subResourse.pSysMem = &minimap_vertex_list_.at(0);
	subResourse.SysMemPitch;
	subResourse.SysMemSlicePitch;

	DX11APP->GetDeviceContext()->UpdateSubresource(minimap_vertex_buf_.Get(), 0, nullptr, &minimap_vertex_list_.at(0), sizeof(UIVertex), 0);
}

void UI_Minimap::RenderExceptTextureSet()
{
	// Set Topology
	DX11APP->GetDeviceContext()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Set Vertex Buffer
	UINT stride = sizeof(UIVertex);
	UINT offset = 0;
	DX11APP->GetDeviceContext()->IASetVertexBuffers(0, 1, render_data_.vertex_buffer.GetAddressOf(), &stride, &offset);

	// Set Index Buffer
	DX11APP->GetDeviceContext()->IASetIndexBuffer(render_data_.index_buffer.Get(), DXGI_FORMAT_R32_UINT, offset);

	// Get Shader From ResourceMgr
	VertexShader* vs = RESOURCE->UseResource<VertexShader>(render_data_.vs_id);
	PixelShader* ps = RESOURCE->UseResource<PixelShader>(render_data_.ps_id);

	if (vs == nullptr || ps == nullptr)
		return;

	// Set InputLayout
	DX11APP->GetDeviceContext()->IASetInputLayout(vs->InputLayout());

	// Set VS
	DX11APP->GetDeviceContext()->VSSetShader(vs->Get(), 0, 0);

	// Set PS
	DX11APP->GetDeviceContext()->PSSetShader(ps->Get(), 0, 0);

	// Reset GS
	DX11APP->GetDeviceContext()->GSSetShader(nullptr, 0, 0);

	// Draw Indexed
	DX11APP->GetDeviceContext()->DrawIndexed(render_data_.index_list.size(), 0, 0);
}

XMVECTOR UI_Minimap::ConvertWorldToScreenXZ(XMVECTOR world)
{
	
	// X
	float world_x = world.m128_f32[0];
	float screen_x = world_x / MINIMAP_LENGTH + 0.5f;

	// Z
	float world_z = world.m128_f32[2];
	float screen_z = screen_z = 0.5f - world_z / MINIMAP_LENGTH;


	XMVECTOR convert_vector = XMVectorSet(screen_x, screen_z, 0, 1.0f);

	return convert_vector;
}
