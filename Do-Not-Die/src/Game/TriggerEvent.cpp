#include "TriggerEvent.h"

using namespace reality;

TriggerEvent::TriggerEvent(entt::entity target_actor, entt::entity trigger_actor, bool begin_or_end)
{
	target_actor_ = target_actor;
	trigger_actor_ = trigger_actor;
	begin_or_end_ = begin_or_end;
}

void TriggerEvent::Process()
{
	if (begin_or_end_ == true) // Begin Overlap
	{
		
	}
	else // End Overlap
	{

	}
};
