#include "Managers/EventManager.h"

#include "Managers/SceneManager.h"
#include "Managers/UIManager.h"
#include "Core/EventType.h"
#include "Actors/Actor.h"
#include "Actors/ActorGroup.h"
#include "Components/AIComponent.h"
#include "Components/AIState.h"
#include "Scenes/Scene.h"
#include "ResourceSystem/FTPremade.h"

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
		FTPremade* premade = static_cast<FTPremade*>(executedEvent.eventData.at(0));

#ifdef FOXTROT_EDITOR
		EditorScene* scene = static_cast<EditorScene*>(executedEvent.eventData.at(1));
#else
		Scene* scene = static_cast<Scene*>(executedEvent.eventData.at(1));
#endif
		premade->AddToScene(scene, FTCore::GetInstance());
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
		SceneManager::GetInstance()->SwitchScene(*static_cast<size_t*>(executedEvent.eventData.at(0)));
		UIManager::GetInstance()->Reset();
	}
	break;
	}
}

EventManager::EventManager()
{
}

EventManager::~EventManager()
{}