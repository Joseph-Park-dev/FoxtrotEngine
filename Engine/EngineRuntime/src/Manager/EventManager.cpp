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
	/// @brief Queues an event for processing at the frame boundary.
	/// @param addedEvent Event copied into the pending queue.
	void EventManager::AddEvent(const FTEvent& addedEvent)
	{
		mEvent->PushBack(addedEvent);
	}

	/// @brief Processes queued lifecycle and scene changes at the frame boundary.
	void EventManager::ProcessEvent()
	{
		for (size_t i = 0; i < mEvent->GetSize(); ++i)
			Execute(mEvent->At(i));
		mEvent->Clear();
	}

	/// @brief Applies the requested event or command to engine state.
	/// @param executedEvent Event whose operation is applied.
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

	/// @brief Initializes the pending event collection.
	/// @note Initializes the :EventManager base or delegates to its constructor.
	EventManager::EventManager()
		: mEvent(DBG_NEW Common::FTDS::DynamicArray<FTEvent>(1))
	{
	}

	/// @brief Completes destruction through the object's inheritance hierarchy.
	EventManager::~EventManager()
	{
	}
} // namespace Core
