#include "Core/TypeDefinitions.h"
#include "Core/EventFunctions.h"
#include "Managers/EventManager.h"
#include "Managers/ResourceManager.h"
#include "Scenes/SceneType.h"
#include "Components/AIState.h"
#include "Actors/ActorGroup.h"
#include "ResourceSystem/FTPremade.h"

void Instantiate(Actor* actor, ActorGroup* actorGroup)
{
	FTEvent addedEvent = {};
	addedEvent.incident = EVENT_TYPE::CREATE_ACTOR;
	addedEvent.eventData.push_back(actor);
	addedEvent.eventData.push_back(actorGroup);
	EventManager::GetInstance()->AddEvent(addedEvent);
}

void Instantiate(const char* premadeName, Scene* scene)
{
	FTEvent addedEvent = {};
	addedEvent.incident = EVENT_TYPE::CREATE_ACTOR;
	FTPremade* premade = ResourceManager::GetInstance()->GetLoadedPremade(std::move(premadeName));
	if (premade)
	{
		addedEvent.eventData.push_back(premade);
		addedEvent.eventData.push_back(scene);
		EventManager::GetInstance()->AddEvent(addedEvent);
	}
	else
		printf("ERROR : Instantiate() -> Premade not loaded, %s\n", premadeName);
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