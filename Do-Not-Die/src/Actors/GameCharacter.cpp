#include "GameCharacter.h"

CharacterStatus::CharacterStatus(float def, float cur, float min, float max)
{
	min_value_ = min;
	max_value_ = max;
	default_value_ = def;
	current_value_ = cur;
}

void CharacterStatus::Update()
{
	is_on_ = Tick();
	if (is_on_)
	{
		current_value_ = default_value_ + variation_;
	}
	else
	{
		current_value_ = default_value_;
		variation_ = 0.0f;
	}

	current_value_ = max(min_value_, current_value_);
	current_value_ = min(max_value_, current_value_);
}

float CharacterStatus::GetMinValue() { return min_value_; }
float CharacterStatus::GetMaxValue() { return max_value_; }
float CharacterStatus::GetCurrentValue() { return current_value_; }
float CharacterStatus::GetDefaultValue() { return default_value_; }
float CharacterStatus::GetVariation() { return variation_; }
float CharacterStatus::GetTimer() { return timer_; }

bool CharacterStatus::GetIsOn(){ return is_on_; }

bool CharacterStatus::Tick()
{
	if (timer_ < 0.001f)
	{
		timer_ = 0.0f;
		return false;
	}

	timer_ -= TM_DELTATIME;		
	return true;	
}

void CharacterStatus::TimeLimitedVariation(float time, float variation)
{
	timer_ = time;
	variation_ = variation;
}
void CharacterStatus::PermanentVariation(float variation)
{
	default_value_ += variation;
}

void CharacterStatus::SetDefualtValue(float value)
{
	default_value_ = current_value_ = value;
}

CharacterStatus* GameCharacter::GetStatus(const string& name)
{
	auto iter = status_map_.find(name);
	if (iter != status_map_.end())
		return &iter->second;
	else
		return nullptr;
}

void GameCharacter::TakeDamage(int damage)
{
	is_hit_ = true;
	GetStatus("hp")->PermanentVariation(-damage);
};

bool GameCharacter::AddStatus(const string& name, const CharacterStatus& status)
{
	if (GetStatus(name) != nullptr)
		return false;

	status_map_.insert(make_pair(name, status));
	return true;
}
void GameCharacter::UpdateStatus()
{
	for (auto& status : status_map_)
	{
		status.second.Update();
	}
}