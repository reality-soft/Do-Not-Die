#include "BaseEnemy.h"
#include "AttackEvent.h"
#include "AnimationStateMachine.h"
#include "Player.h"
#include "BossZombie_ASM.h"
#include "BaseEnemy_BTN.h"

void BaseEnemy::OnUpdate()
{
	ChasePlayer();
	behavior_tree_.Update();
	Character::OnUpdate();
}

void BaseEnemy::ChasePlayer()
{
	if (in_defense_bound_ == false)
		return;

	auto player = SCENE_MGR->GetPlayer<Player>(0);
	if (player == nullptr)
		return;

	auto c_enemy_capsule = reg_scene_->try_get<C_CapsuleCollision>(entity_id_);
	if (c_enemy_capsule == nullptr)
		return;

	auto c_player_capsule = reg_scene_->try_get<C_CapsuleCollision>(player->entity_id_);
	if (c_player_capsule == nullptr)
		return;

	RayShape sight_ray;
	sight_ray.start = _XMFLOAT3(GetTipBaseAB(c_enemy_capsule->capsule)[3]);
	sight_ray.end = _XMFLOAT3(GetTipBaseAB(c_player_capsule->capsule)[3]);

	auto callback = QUADTREE->RaycastCarOnly(sight_ray);
	if (callback.success)
		player_in_sight_ = false;
	else
		player_in_sight_ = true;
}

float BaseEnemy::GetMaxHp() const
{
	return max_hp_;
}

float BaseEnemy::GetCurHp() const
{
	return cur_hp_;
}

void BaseEnemy::SetCurHp(int hp)
{
	cur_hp_ = hp;
}

void BaseEnemy::TakeDamage(int damage)
{
	is_hit_ = true;
	cur_hp_ -= damage;
}

void BaseEnemy::SetMovement(const XMVECTOR& direction)
{
	if (XMVector3Length(direction).m128_f32[0] <= 0.00001f) {
		return;
	}

	XMVECTOR dir = direction; dir.m128_f32[1] = 0.0f;

	is_moving_ = true;
	XMVECTOR front = { 0.0f, 0.0f, 1.0f, 0.0f };
	XMVECTOR right = { 1.0f, 0.0f, 0.0f, 0.0f };

	float dot_product = XMVectorGetX(XMVector3Dot(front, dir));
	float angle = XMVectorGetX(XMVector3AngleBetweenVectors(front, dir));
	if (XMVectorGetX(XMVector3Dot(right, dir)) < 0)
		angle = XM_2PI - angle;

	rotation_ = XMMatrixRotationY(angle);

	GetMovementComponent()->accelaration_vector[2] = 1;
}
