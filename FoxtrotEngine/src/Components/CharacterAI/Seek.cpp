#include "Seek.h"

#include "Actors/Actor.h"
#include "Actors/Transform.h"

#ifdef FOXTROT_EDITOR
#include "EditorSceneManager.h"
#include "EditorElement.h"
#endif // FOXTROT_EDITOR

std::string Seek::GetName() const
{
    return "Seek";
}

Steering Seek::CreateSteering()
{
    Steering steering = Steering::Halt();
    steering.Linear = 
        GetTarget()->GetTransform()->GetWorldPosition() -
        GetOwner()->GetTransform()->GetWorldPosition();
    FTVector3::Normalize(steering.Linear);

    return steering;
}

void Seek::CloneTo(Actor* actor)
{
    Seek* newComp = DBG_NEW Seek(actor, GetUpdateOrder());
    newComp->SetTargetName(this->GetTargetName());
}

Seek::Seek(Actor* owner, int updateOrder)
    : AgentBehavior(owner, updateOrder)
{}