#pragma once
#include "Engine_include.h"
#include "AnimationStateMachine.h"
#include "AnimationState.h"

class GameCharacter : public reality::Character
{
public:
	virtual float GetCharacterDamage() {
		return 0.0f;
	};

public:
	virtual float GetMaxHp() const { 
		return max_hp_; 
	};
	virtual void SetCurHp(int hp) {};
	virtual void TakeDamage(int damage) {
		cur_hp_ -= damage;
	};
	virtual float GetCurHp() const {
		return cur_hp_;
	};

protected:
	float max_hp_;
	float cur_hp_;
};

