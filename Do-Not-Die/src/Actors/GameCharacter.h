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
	void SetDefualtValue(float value);

	float current_value_ = 0;
private:
	bool Tick();

	float timer_ = 0.0f;
	float min_value_ = 0;
	float max_value_ = 0;
	float default_value_ = 0;
	float variation_ = 0;

};

class GameCharacter : public reality::Character
{
public:
	bool is_hit_ = false;
	virtual void TakeDamage(int damage);


	CharacterStatus* GetStatus(const string& name);
	bool AddStatus(const string& name, const CharacterStatus& status);
	void UpdateStatus();

protected:
	map<string, CharacterStatus> status_map_;
};