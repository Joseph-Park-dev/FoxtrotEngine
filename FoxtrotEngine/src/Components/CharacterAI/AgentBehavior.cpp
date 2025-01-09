#include "Components/CharacterAI/AgentBehavior.h"

#include "Actors/Actor.h"
#include "Actors/Transform.h"

Steering AgentBehavior::CreateSteering()
{
	return Steering::Halt();
}

void AgentBehavior::SetTarget(Transform* target)
{
	mTarget = target;
}

void AgentBehavior::Initialize(FTCore* coreInstance)
{
	Component::Initialize(coreInstance);
}

void AgentBehavior::Setup()
{
	Component::Setup();
}

void AgentBehavior::Update(float deltaTime)
{
	GetOwner()->GetTransform()->SetSteering(CreateSteering());
}

AgentBehavior::AgentBehavior(Actor* owner, int drawOrder, int updateOrder)
	: Component	(owner, drawOrder, updateOrder)
	, mTarget	(nullptr)
{}

AgentBehavior::~AgentBehavior()
{}