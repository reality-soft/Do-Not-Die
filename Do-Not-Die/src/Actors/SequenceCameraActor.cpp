#include "SequenceCameraActor.h"
#include "FileTransfer.h"

void SequenceCameraActor::OnInit(entt::registry& registry)
{
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
}

void SequenceCameraActor::OnUpdate()
{
	PlaySequence(30);
	
	world_matrix_ = XMMatrixAffineTransformation(XMVectorReplicate(1.0f), XMVectorZero(), rotation_, world_pos_);
	view_matrix_ = XMMatrixInverse(0, world_matrix_);

	cb_camera_info_.data.view_proj_matrix = XMMatrixTranspose(view_matrix_ * proj_matrix_);
	cb_camera_info_.data.camera_position = world_pos_;
	cb_camera_info_.data.camera_position.m128_f32[3] = 100000.0f;
	cb_camera_info_.data.camera_translation = XMMatrixTranspose(XMMatrixTranslationFromVector(world_pos_));
	cb_camera_info_.data.camera_look = world_matrix_.r[2];

	DX11APP->GetDeviceContext()->UpdateSubresource(cb_camera_info_.buffer.Get(), 0, nullptr, &cb_camera_info_.data, 0, 0);
	DX11APP->GetDeviceContext()->VSSetConstantBuffers(0, 1, cb_camera_info_.buffer.GetAddressOf());
}

bool SequenceCameraActor::PlaySequence(float time)
{
	if (current_track_ >= sequence_tracks_.size())
		return false;

	static float sequence_time = 0.0f;
	if (sequence_time > time)
		return false;


	float speed = 10;
	auto track = sequence_tracks_[current_track_];
	static float move_legnth = 0.0f;
	
	XMVECTOR movement = track.move_vector * speed * TM_DELTATIME;
	world_pos_ += movement;
	float current_pitch = pitch + track.rotate_pitch * lerp_value;
	float current_yaw = yaw + track.rotate_yaw * lerp_value;
	rotation_ = XMQuaternionRotationRollPitchYaw(current_pitch, current_yaw, 0);

	move_legnth += Vector3Length(movement);
	if (move_legnth > track.length)
	{
		current_track_++;
		pitch = current_pitch;
		yaw = current_yaw;
		move_legnth = 0;
	}

	lerp_value = move_legnth / track.length;

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
		track_length += track.length;
	}

	world_pos_ = start_cut.world_pos;
	pitch = start_cut.camera_pitch;
	yaw = start_cut.camera_yaw;
}