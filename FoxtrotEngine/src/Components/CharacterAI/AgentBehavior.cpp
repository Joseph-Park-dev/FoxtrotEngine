#include "Components/CharacterAI/AgentBehavior.h"

#include "Actors/Actor.h"
#include "Actors/Transform.h"

Steering AgentBehavior::CreateSteering()
{
	return Steering::Halt();
}

void AgentBehavior::Initialize(FTCore* coreInstance)
{
	mAgent = this->GetOwner()->GetTransform();
	Component::Initialize(coreInstance);
}

void AgentBehavior::Setup()
{
	Component::Setup();
}

void AgentBehavior::Update(float deltaTime)
{
	mAgent->SetSteering(CreateSteering());
}

AgentBehavior::AgentBehavior()
	: Component	(GetOwner(), DEFAULT_DRAWORDER, DEFAULT_UPDATEORDER)
	, mAgent	(nullptr)
	, mTarget	(nullptr)
{}

AgentBehavior::~AgentBehavior()
{}