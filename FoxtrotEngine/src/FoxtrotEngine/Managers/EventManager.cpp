#include "FoxtrotEngine/Managers/EventManager.h"

#include "FoxtrotEngine/Managers/SceneManager.h"
#include "FoxtrotEngine/Managers/UIManager.h"
#include "FoxtrotEngine/Core/EventType.h"
#include "FoxtrotEngine/Actors/Actor.h"
#include "FoxtrotEngine/Actors/ActorGroup.h"
#include "FoxtrotEngine/Components/AIComponent.h"
#include "FoxtrotEngine/Components/AIState.h"
#include "FoxtrotEngine/Scenes/Scene.h"

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
