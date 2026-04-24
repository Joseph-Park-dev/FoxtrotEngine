// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Manager/EventManager.h"

#include "Manager/SceneManager.h"
#include "EventType.h"
#include "Actor/Actor.h"
#include "Actor/ActorGroup.h"
#include "Scene/Scene.h"
#include "ResourceSystem/FTPremade.h"
#include "Dynamic/DynamicArray.h"

#ifdef FOXTROT_EDITOR
	#include "EditorSceneManager.h"
	#include "EditorElement.h"
	#include "EditorScene.h"
#endif // FOXTROT_EDITOR

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

#ifdef FOXTROT_EDITOR
				Editor::EditorElement* created = static_cast<Editor::EditorElement*>(executedEvent.eventData);
				Editor::EditorScene*   scene   = Editor::EditorSceneManager::GetInstance()->GetEditorScene();
				scene->AddEditorElement(created);
#else
				Actor* created = static_cast<Actor*>(executedEvent.eventData);
				Scene* scene   = SceneManager::GetInstance()->GetCurrentScene();
				scene->AddActor(created);
#endif
			}
			break;

			case EVENT_TYPE::DESTROY_ACTOR:
			{

#ifdef FOXTROT_EDITOR
				Editor::EditorElement* element = static_cast<Editor::EditorElement*>(executedEvent.eventData);
				Editor::EditorScene*   scene   = Editor::EditorSceneManager::GetInstance()->GetEditorScene();
				scene->RemoveActor(element);
#else
				Actor* actorToDestroy = static_cast<Actor*>(executedEvent.eventData);
				actorToDestroy->SetState(ActorState::DEAD);
#endif
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
		: mEvent(DBG_NEW FTDS::DynamicArray<FTEvent>(1))
	{
	}

	EventManager::~EventManager()
	{
	}
} // namespace Core