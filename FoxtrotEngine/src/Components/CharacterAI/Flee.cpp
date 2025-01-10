#include "Components/CharacterAI/Flee.h"

#include "Actors/Actor.h"

#ifdef FOXTROT_EDITOR
#include "EditorUtils.h"
#endif

std::string Flee::GetName() const
{
    return "Flee";
}

Steering Flee::CreateSteering()
{
    Steering steering = Steering::Halt();
    steering.Linear =
        GetOwner()->GetTransform()->GetWorldPosition() -
        GetTarget()->GetTransform()->GetWorldPosition();
    FTVector3::Normalize(steering.Linear);

    return steering;
}

void Flee::CloneTo(Actor* actor)
{
    Flee* newComp = DBG_NEW Flee(actor, GetDrawOrder(), GetUpdateOrder());
    newComp->SetTargetName(this->GetTargetName());
}

Flee::Flee(Actor* owner, int drawOrder, int updateOrder)
	: AgentBehavior(owner, drawOrder, updateOrder)
{}