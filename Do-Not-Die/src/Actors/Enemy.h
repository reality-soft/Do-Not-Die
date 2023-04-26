#pragma once
#include "GameCharacter.h"
#include "Player.h"

class Enemy : public GameCharacter
{
public:
	virtual void OnInit(entt::registry& registry) override;
	virtual void OnUpdate() override;
	void SetCharacterAnimation(string anim_id) const;

public:
	void SetMovement(const XMVECTOR& direction);
	void Jump();
	void Idle();
	void Attack();

public:
	float	 GetMaxHp() const override;
	float	 GetCurHp() const override;
	void SetCurHp(int hp) override;
	void TakeDamage(int damage) override;

public:
	virtual void SetBehaviorTree(const vector<XMVECTOR>& target_poses);
	void SetMeshId(const string& mesh_id);
	void ChasePlayer();

private:
	string mesh_id_;
	int cur_node = 0;

private:
	reality::BehaviorTree behavior_tree_;

public:
	bool is_hit_ = false;
	bool in_defense_bound_ = false;
	bool player_in_sight_ = true;
	bool is_attacking_ = false;
	bool is_attack_ended = false;

private:
	bool is_moving_ = false;

public:
	bool IsMoving() { return is_moving_; }
};
