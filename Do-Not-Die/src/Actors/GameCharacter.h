#pragma once
#include "Engine_include.h"
#include "AnimationStateMachine.h"
#include "AnimationState.h"

class CharacterStatus
{
public:
	CharacterStatus(float def, float cur, float min, float max);
	void Update();

	float GetMinValue();
	float GetMaxValue();
	float GetCurrentValue();
	float GetDefaultValue();
	float GetVariation();
	int GetTimer();
	void TimeLimitedVariation(float time, float variation);
	void PermanentVariation(float variation);

	float timer_ = 0.0f;
private:
	bool Tick();

	float current_value_ = 0;
	float min_value_ = 0;
	float max_value_ = 0;
	float default_value_ = 0;
	float variation_ = 0;

};

class GameCharacter : public reality::Character
{
public:
	bool is_hit_ = false;

	//virtual float GetCharacterDamage() {
	//	return damage_;
	//};

	//virtual float GetMaxHp() const { 
	//	return max_hp_; 
	//};
	//virtual void SetCurHp(int hp) {};
	virtual void TakeDamage(int damage) {
		is_hit_ = true;
		GetStatus("hp")->PermanentVariation(-damage);// -= damage;
	};
	//virtual float GetCurHp() const {
	//	return cur_hp_;
	//};

	CharacterStatus* GetStatus(const string& name);
	bool AddStatus(const string& name, const CharacterStatus& status);
	void UpdateStatus();

protected:
	//float damage_;
	//float max_hp_;
	//float cur_hp_;

	map<string, CharacterStatus> status_map_;
};