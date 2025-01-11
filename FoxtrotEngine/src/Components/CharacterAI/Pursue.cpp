#include "Components/CharacterAI/Pursue.h"

void Pursue::Setup()
{
	AgentBehavior::Setup();
	mTargetAux = GetTarget();
}
