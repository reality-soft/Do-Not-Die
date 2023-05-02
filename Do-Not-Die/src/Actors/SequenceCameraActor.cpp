#include "SequenceCameraActor.h"
#include "FileTransfer.h"

void SequenceCameraActor::OnInit(entt::registry& registry)
{
	Actor::OnInit(registry);

	auto viewport = DX11APP->GetViewPortAddress();
	float aspect = viewport->Width / viewport->Height;
	float fov = XMConvertToRadians(45);
	proj_matrix_ = XMMatrixPerspectiveFovLH(fov, aspect, 1.0f, 100000.0f);

	D3D11_BUFFER_DESC desc;
	D3D11_SUBRESOURCE_DATA subdata;

	ZeroMemory(&desc, sizeof(desc));
	ZeroMemory(&subdata, sizeof(subdata));

	desc.ByteWidth = sizeof(CbCameraInfo::Data);
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	subdata.pSysMem = cb_camera_info_.buffer.GetAddressOf();

	HRESULT hr = DX11APP->GetDevice()->CreateBuffer(&desc, &subdata, cb_camera_info_.buffer.GetAddressOf());
	if (FAILED(hr))
		return;

	ZeroMemory(&desc, sizeof(desc));
	ZeroMemory(&subdata, sizeof(subdata));

	desc.ByteWidth = sizeof(CbCameraEffect::Data);
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	subdata.pSysMem = cb_effect.buffer.GetAddressOf();

	hr = DX11APP->GetDevice()->CreateBuffer(&desc, &subdata, cb_effect.buffer.GetAddressOf());
	if (FAILED(hr))
		return;
}

void SequenceCameraActor::OnUpdate()
{
	if (playable)
		PlaySequence();

	cb_camera_info_.data.view_proj_matrix = XMMatrixTranspose(view_matrix_ * proj_matrix_);
	cb_camera_info_.data.camera_position = world_pos_;
	cb_camera_info_.data.camera_position.m128_f32[3] = 100000.0f;
	cb_camera_info_.data.camera_translation = XMMatrixTranspose(XMMatrixTranslationFromVector(world_pos_));
	cb_camera_info_.data.camera_look = world_matrix_.r[2];

	DX11APP->GetDeviceContext()->UpdateSubresource(cb_camera_info_.buffer.Get(), 0, nullptr, &cb_camera_info_.data, 0, 0);
	DX11APP->GetDeviceContext()->VSSetConstantBuffers(0, 1, cb_camera_info_.buffer.GetAddressOf());

	// Billboard
	cb_effect.data.view_matrix = XMMatrixTranspose(view_matrix_);
	cb_effect.data.projection_matrix = XMMatrixTranspose(proj_matrix_);
	XMMATRIX billboard = XMMatrixInverse(0, view_matrix_);
	billboard.r[3].m128_f32[0] = 0.0f;
	billboard.r[3].m128_f32[1] = 0.0f;
	billboard.r[3].m128_f32[2] = 0.0f;
	cb_effect.data.main_billboard = XMMatrixTranspose(billboard);

	XMMATRIX x_only = XMMatrixIdentity();
	x_only.r[1].m128_f32[1] = view_matrix_.r[1].m128_f32[1];
	x_only.r[1].m128_f32[2] = view_matrix_.r[1].m128_f32[2];
	x_only.r[2].m128_f32[1] = view_matrix_.r[2].m128_f32[1];
	x_only.r[2].m128_f32[2] = view_matrix_.r[2].m128_f32[2];

	billboard = DirectX::XMMatrixInverse(0, x_only);
	billboard.r[3].m128_f32[0] = 0.0f;
	billboard.r[3].m128_f32[1] = 0.0f;
	billboard.r[3].m128_f32[2] = 0.0f;
	cb_effect.data.x_billboard = XMMatrixTranspose(billboard);

	XMMATRIX y_only = XMMatrixIdentity();
	y_only.r[0].m128_f32[0] = view_matrix_.r[0].m128_f32[0];
	y_only.r[0].m128_f32[2] = view_matrix_.r[0].m128_f32[2];
	y_only.r[2].m128_f32[0] = view_matrix_.r[2].m128_f32[0];
	y_only.r[2].m128_f32[2] = view_matrix_.r[2].m128_f32[2];

	billboard = DirectX::XMMatrixInverse(0, y_only);
	billboard.r[3].m128_f32[0] = 0.0f;
	billboard.r[3].m128_f32[1] = 0.0f;
	billboard.r[3].m128_f32[2] = 0.0f;
	cb_effect.data.y_billboard = XMMatrixTranspose(billboard);

	DX11APP->GetDeviceContext()->UpdateSubresource(cb_effect.buffer.Get(), 0, nullptr, &cb_effect.data, 0, 0);
	DX11APP->GetDeviceContext()->GSSetConstantBuffers(0, 1, cb_effect.buffer.GetAddressOf());
}

bool SequenceCameraActor::PlaySequence()
{
	if (current_track_ >= sequence_tracks_.size() && keep_sequence_at_last_direction == false)
		return false;

	// Lerp Speed By Time
	static float speed = 0.f;
	static float time = 0.0f;
	time += TM_DELTATIME;
	float lerp_time = time / 20;
	speed = play_speed * pow(pow(2.71828182846, time), 2 * lerp_time) * acc;
	speed = min(play_speed, speed);
	

	auto track = sequence_tracks_[current_track_];
	
	XMVECTOR movement_vector = XMVector3Normalize(track.move_vector);
	float track_length = Vector3Length(track.move_vector);
	
	XMVECTOR movement = movement_vector * speed * TM_DELTATIME;
	world_pos_ += movement;
	float current_pitch = pitch + track.rotate_pitch_yaw.x * lerp_value;
	float current_yaw = yaw + track.rotate_pitch_yaw.y * lerp_value;
	rotation_ = XMQuaternionRotationRollPitchYaw(current_pitch, current_yaw, 0);

	static float move_legnth = 0.0f;
	move_legnth += Vector3Length(movement);

	if (move_legnth > track_length)
	{
		pitch = current_pitch;
		yaw = current_yaw;
		move_legnth = 0;

		if (keep_sequence_at_last_direction == false && current_track_ < sequence_tracks_.size() - 1)
		{
			current_track_++;
		}
	}

	lerp_value = move_legnth / Vector3Length(track.move_vector);

	switch (track.sequence_type)
	{
	case SequenceType::ViewSequence:
		world_matrix_ = XMMatrixAffineTransformation(XMVectorReplicate(1.0f), XMVectorZero(), rotation_, world_pos_);
		view_matrix_ = XMMatrixInverse(0, world_matrix_);
		break;
	case SequenceType::TargetSequence:
		view_matrix_ = XMMatrixLookAtLH(world_pos_, track.sequence_target, XMVectorSet(0, 1, 0, 0));
		world_matrix_ = XMMatrixInverse(0, view_matrix_);
	}

	return true;
}

void SequenceCameraActor::ImportSequenceTrack(string mapdat_file)
{
	FileTransfer in_mapdata("../../Contents/BinaryPackage/" + mapdat_file, READ);
	 
	SequenceCutInfo start_cut;
	in_mapdata.ReadBinary<SequenceCutInfo>(start_cut);

	UINT num_tracks;
	in_mapdata.ReadBinary<UINT>(num_tracks);

	for (UINT i = 0; i < num_tracks; ++i)
	{
		SequenceTrack track;
		in_mapdata.ReadBinary<SequenceTrack>(track);

		sequence_tracks_.push_back(track);
	}

	for (const auto& track : sequence_tracks_)
	{
		track_length += Vector3Length(track.move_vector);
	}

	world_pos_ = start_cut.world_pos;
	pitch = start_cut.camera_pitch;
	yaw = start_cut.camera_yaw;
}