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
	FTDS::String msg;
	msg.Assign(GetOwner()->GetNameRef());
	msg.Append(" : Collision Entered with ");
	msg.Append(other->GetOwner()->GetName());
	LogString(msg);
}

void FTBehavior::OnCollisionStay(Collider2D* other)
{
}

void FTBehavior::OnCollisionExit(Collider2D* other)
{
	FTDS::String msg;
	msg.Assign(GetOwner()->GetNameRef());
	msg.Append(" : Collision Exited with ");
	msg.Append(other->GetOwner()->GetName());
	LogString(msg);
}