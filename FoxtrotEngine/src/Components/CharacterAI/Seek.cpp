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
        GetTarget()->GetWorldPosition() -
        GetOwner()->GetTransform()->GetWorldPosition();
    FTVector3::Normalize(steering.Linear);

    return steering;
}

void Seek::Initialize(FTCore* ftCoreInstance)
{
    AgentBehavior::Initialize(ftCoreInstance);
}

void Seek::Setup()
{
#ifdef FOXTROT_EDITOR
    Actor* actor = EditorSceneManager::GetInstance()->GetEditorScene()->FindActor("Henry");
    SetTarget(actor->GetTransform());
#endif // FOXTROT_EDITOR
    AgentBehavior::Setup();
}

void Seek::CloneTo(Actor* actor)
{
    Seek* newComp = DBG_NEW Seek(actor, GetDrawOrder(), GetUpdateOrder());
}

Seek::Seek(Actor* owner, int drawOrder, int updateOrder)
    : AgentBehavior(owner, drawOrder, updateOrder)
{}