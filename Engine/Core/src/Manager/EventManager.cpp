// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Manager/EventManager.h"

#include "Manager/SceneManager.h"
#include "EventSystem/EventType.h"
#include "Actor/Actor.h"
#include "Actor/ActorData.h"
#include "Actor/ActorGroup.h"
#include "Scene/Scene.h"
#include "ResourceSystem/FTPremade.h"
#include "FTDS/Dynamic/DynamicArray.h"

namespace Core
{
	void EventManager::AddEvent(const FTEvent& addedEvent)
	{
		mEvent->PushBack(addedEvent);
	}

	void EventManager::ProcessEvent()
	{
		for (size_t i = 0; i < mEvent->GetSize(); ++i)
			Execute(mEvent->At(i));
		mEvent->Clear();
	}

	void EventManager::Execute(const FTEvent& executedEvent)
	{
		switch (executedEvent.incident)
		{
			case EVENT_TYPE::CREATE_ACTOR:
			{
				Actor* created = static_cast<Actor*>(executedEvent.eventData);
				Scene* scene   = SceneManager::GetInstance()->GetCurrentScene();
				scene->AddActor(created);
			}
			break;

			case EVENT_TYPE::DESTROY_ACTOR:
			{
				Actor* actorToDestroy = static_cast<Actor*>(executedEvent.eventData);
				actorToDestroy->SetState(Common::ActorState::DEAD);
			}
			break;
			case EVENT_TYPE::SWITCH_SCENE:
			{
				SceneManager::GetInstance()->SwitchScene(*static_cast<size_t*>(executedEvent.eventData));
			}
			break;
		}
	}

	EventManager::EventManager()
		: mEvent(DBG_NEW Common::FTDS::DynamicArray<FTEvent>(1))
	{
	}

	EventManager::~EventManager()
	{
	}
} // namespace Core