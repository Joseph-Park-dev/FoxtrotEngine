#include "Managers/EventManager.h"

#include "Managers/SceneManager.h"
#include "Managers/UIManager.h"
#include "Core/EventType.h"
#include "Actors/Actor.h"
#include "Actors/ActorGroup.h"
#include "Components/AIComponent.h"
#include "Components/AIState.h"
#include "Scenes/Scene.h"

#ifdef FOXTROT_EDITOR
#include "EditorSceneManager.h"
#endif // FOXTROT_EDITOR


void EventManager::ProcessEvent()
{
	for (size_t i = 0; i < mEvent.size(); ++i)
	{
		Execute(mEvent[i]);
	}
	mEvent.clear();
}

void EventManager::Execute(const FTEvent& executedEvent)
{
	switch (executedEvent.incident)
	{
	case EVENT_TYPE::CREATE_ACTOR:
	{
		Actor* newActor = static_cast<Actor*>(executedEvent.eventData.at(0));
		ActorGroup group = *(static_cast<ActorGroup*>(executedEvent.eventData.at(1)));
		newActor->SetActorGroup(group);
#ifdef FOXTROT_EDITOR
		EditorSceneManager::GetInstance()->GetEditorScene()->AddEditorElement(newActor);
#else
		SceneManager::GetInstance()->GetCurrentScene()->AddActor(newActor, group);
#endif // FOXTROT_EDITOR

	}
	break;
	case EVENT_TYPE::DESTROY_ACTOR:
	{
		Actor* actorToDestroy = static_cast<Actor*>(executedEvent.eventData.at(0));
		actorToDestroy->SetState(Actor::State::EDead);
	}
	break;
	case EVENT_TYPE::SWITCH_SCENE:
	{
		SceneManager::GetInstance()->SwitchScene(*static_cast<SCENE_TYPE*>(executedEvent.eventData.at(0)));
		UIManager::GetInstance()->Reset();
	}
	break;
	}
}

EventManager::EventManager()
{
}

EventManager::~EventManager()
{
}
