#pragma once
#include "Engine_Include.h"

using namespace reality;

enum class SequenceType
{
	ViewSequence,
	TargetSequence,
};

struct SequenceTrack
{
	SequenceType sequence_type;
	XMVECTOR move_vector;
	XMFLOAT2 rotate_pitch_yaw;
	XMVECTOR sequence_target;
};

struct SequenceCutInfo
{
	XMVECTOR world_pos;
	float camera_pitch;
	float camera_yaw;
};

class SequenceCameraActor : public Actor
{
public:
	virtual void OnInit(entt::registry& registry);
	virtual void OnUpdate();

	bool PlaySequence();
	bool playable = false;
	
	void ImportSequenceTrack(string mapdat_file);

	UINT current_track_ = 0;
	vector<SequenceTrack> sequence_tracks_;
	float track_length = 0;
	float lerp_value = 0;
	float acc = 0.01f;
	float play_speed = 30.0f;
	bool keep_sequence_at_last_direction = true;

	XMVECTOR world_pos_;
	float yaw;
	float pitch;
	XMVECTOR rotation_;

private:
	CbCameraInfo cb_camera_info_;
	CbCameraEffect	cb_effect;

	XMMATRIX world_matrix_;
	XMMATRIX view_matrix_;
	XMMATRIX proj_matrix_;
};

