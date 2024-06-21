#include "FoxtrotEngine/Core/TypeDefinitions.h"
#include "FoxtrotEngine/Core/EventFunctions.h"
#include "FoxtrotEngine/Managers/EventManager.h"
#include "FoxtrotEngine/Scenes/SceneType.h"
#include "FoxtrotEngine/Components/AIState.h"
#include "FoxtrotEngine/Actors/ActorGroup.h"

void Instantiate(Actor* actor, ACTOR_GROUP actorGroup)
{
	Event addedEvent = {};
	addedEvent.incident = EVENT_TYPE::CREATE_ACTOR;
	addedEvent.lParam = (ULONG_PTR)actor;
	addedEvent.wParam = (ULONG_PTR)actorGroup;
	EventManager::GetInstance()->AddEvent(addedEvent);
}

void Destroy(Actor* actor)
{
	Event addedEvent = {};
	addedEvent.incident = EVENT_TYPE::DESTROY_ACTOR;
	addedEvent.lParam = (ULONG_PTR)actor;
	EventManager::GetInstance()->AddEvent(addedEvent);
}

void SwitchScene(SCENE_TYPE sceneType)
{
	Event addedEvent = {};
	addedEvent.incident = EVENT_TYPE::SWITCH_SCENE;
	addedEvent.lParam = (ULONG_PTR)sceneType;
	EventManager::GetInstance()->AddEvent(addedEvent);
}

void ChangeAIState(AIComponent* ai, AISTATE_TYPE stateType)
{
	Event addedEvent = {};
	addedEvent.incident = EVENT_TYPE::CHANGE_AISTATE;
	addedEvent.lParam = (ULONG_PTR)ai;
	addedEvent.wParam = (ULONG_PTR)stateType;
	EventManager::GetInstance()->AddEvent(addedEvent);
}