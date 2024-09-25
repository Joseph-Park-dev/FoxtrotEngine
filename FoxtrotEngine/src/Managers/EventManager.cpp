#include "Managers/EventManager.h"

#include "Managers/SceneManager.h"
#include "Managers/UIManager.h"
#include "Core/EventType.h"
#include "Actors/Actor.h"
#include "Actors/ActorGroup.h"
#include "Components/AIComponent.h"
#include "Components/AIState.h"
#include "Scenes/Scene.h"

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
		Actor* newActor = (Actor*)executedEvent.lParam;
		ACTOR_GROUP group = (ACTOR_GROUP)executedEvent.wParam;
		newActor->SetActorGroup(group);
		SceneManager::GetInstance()->GetCurrScene()->AddActor(newActor, group);
	}
	break;
	case EVENT_TYPE::DESTROY_ACTOR:
	{
		Actor* destroyedActor = (Actor*)executedEvent.lParam;
		destroyedActor->SetState(Actor::State::EDead);
	}
	break;
	case EVENT_TYPE::SWITCH_SCENE:
	{
		SceneManager::GetInstance()->SwitchScene((SCENE_TYPE)executedEvent.lParam);
		UIManager::GetInstance()->Reset();
	}
	break;
	case EVENT_TYPE::CHANGE_AISTATE:
	{
		AIComponent* ai = (AIComponent*)executedEvent.lParam;
		AISTATE_TYPE state = (AISTATE_TYPE)executedEvent.wParam;
		ai->ChangeState(state);
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
