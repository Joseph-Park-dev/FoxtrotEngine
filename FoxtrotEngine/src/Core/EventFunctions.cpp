#include "Core/TypeDefinitions.h"
#include "Core/EventFunctions.h"
#include "Managers/EventManager.h"
#include "Managers/ResourceManager.h"
#include "Scenes/SceneType.h"
#include "Components/AIState.h"
#include "Actors/ActorGroup.h"
#include "Actors/Actor.h"
#include "ResourceSystem/FTPremade.h"

#ifdef FOXTROT_EDITOR
#include "FTCoreEditor.h"
#endif // FOXTROT_EDITOR



void Instantiate(Actor* actor, ActorGroup actorGroup)
{
	FTEvent addedEvent = {};
	addedEvent.incident = EVENT_TYPE::CREATE_ACTOR;
	actor->SetActorGroup(actorGroup);
	addedEvent.eventData.push_back(actor);
	EventManager::GetInstance()->AddEvent(addedEvent);
}

FTPremade* Instantiate(const char* premadeName)
{
	FTEvent addedEvent = {};
	addedEvent.incident = EVENT_TYPE::CREATE_ACTOR;
	FTPremade* premade = ResourceManager::GetInstance()->GetLoadedPremade(std::move(premadeName));

#ifdef FOXTROT_EDITOR
	premade->GetOrigin()->Initialize(FTCoreEditor::GetInstance());
#else
	premade->GetOrigin()->Initialize(FTCore::GetInstance());
#endif // FOXTROT_EDITOR

	if (premade)
	{
		addedEvent.eventData.push_back(premade);
		addedEvent.eventData.push_back(nullptr);
		EventManager::GetInstance()->AddEvent(addedEvent);
		return premade;
	}
	else
	{
		printf("ERROR : Instantiate() -> Premade not loaded, %s\n", premadeName);
		return nullptr;
	}
}

void Destroy(Actor* actor)
{
	FTEvent addedEvent = {};
	addedEvent.incident = EVENT_TYPE::DESTROY_ACTOR;
	addedEvent.eventData.push_back(actor);
	EventManager::GetInstance()->AddEvent(addedEvent);
}

void SwitchScene(SCENE_TYPE* sceneType)
{
	FTEvent addedEvent = {};
	addedEvent.incident = EVENT_TYPE::SWITCH_SCENE;
	addedEvent.eventData.push_back(sceneType);
	EventManager::GetInstance()->AddEvent(addedEvent);
}