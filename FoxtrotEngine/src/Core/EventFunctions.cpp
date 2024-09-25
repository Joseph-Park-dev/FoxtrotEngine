#include "Core/TypeDefinitions.h"
#include "Core/EventFunctions.h"
#include "Managers/EventManager.h"
#include "Scenes/SceneType.h"
#include "Components/AIState.h"
#include "Actors/ActorGroup.h"

void Instantiate(Actor* actor, ACTOR_GROUP actorGroup)
{
	FTEvent addedEvent = {};
	addedEvent.incident = EVENT_TYPE::CREATE_ACTOR;
	addedEvent.lParam = (ULONG_PTR)actor;
	addedEvent.wParam = (ULONG_PTR)actorGroup;
	EventManager::GetInstance()->AddEvent(addedEvent);
}

void Destroy(Actor* actor)
{
	FTEvent addedEvent = {};
	addedEvent.incident = EVENT_TYPE::DESTROY_ACTOR;
	addedEvent.lParam = (ULONG_PTR)actor;
	EventManager::GetInstance()->AddEvent(addedEvent);
}

void SwitchScene(SCENE_TYPE sceneType)
{
	FTEvent addedEvent = {};
	addedEvent.incident = EVENT_TYPE::SWITCH_SCENE;
	addedEvent.lParam = (ULONG_PTR)sceneType;
	EventManager::GetInstance()->AddEvent(addedEvent);
}

void ChangeAIState(AIComponent* ai, AISTATE_TYPE stateType)
{
	FTEvent addedEvent = {};
	addedEvent.incident = EVENT_TYPE::CHANGE_AISTATE;
	addedEvent.lParam = (ULONG_PTR)ai;
	addedEvent.wParam = (ULONG_PTR)stateType;
	EventManager::GetInstance()->AddEvent(addedEvent);
}