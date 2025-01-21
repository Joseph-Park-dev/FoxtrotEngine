#include "Behaviors/FTBehavior.h"

#include "Components/Collider2D.h"
#include "Actors/Actor.h"

FTBehavior::FTBehavior(Actor* owner, int updateOrder)
	: Component(owner, updateOrder)
{}

FTBehavior::~FTBehavior()
{}

void FTBehavior::OnCollisionEnter(Collider2D* other)
{
	LogString(GetOwner()->GetName() + " : Collision Enter with " + other->GetOwner()->GetName());
}

void FTBehavior::OnCollisionStay(Collider2D* other)
{
}

void FTBehavior::OnCollisionExit(Collider2D* other)
{
	LogString(GetOwner()->GetName() + " : Collision Exit with " + other->GetOwner()->GetName());
}