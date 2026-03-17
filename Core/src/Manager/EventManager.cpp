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
#include "FTCoreEditor.h"
#include "EditorSceneManager.h"
#include "EditorElement.h"
#endif // FOXTROT_EDITOR

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
		EditorElement* created = static_cast<EditorElement*>(executedEvent.eventData.at(0));
		EditorScene* scene = EditorSceneManager::GetInstance()->GetEditorScene();
		scene->AddEditorElement(created);
#else
		Actor* created = static_cast<Actor*>(executedEvent.eventData);
		Scene* scene = SceneManager::GetInstance()->GetCurrentScene();
		scene->AddActor(created);
#endif
	}
	break;

	case EVENT_TYPE::DESTROY_ACTOR:
	{

#ifdef FOXTROT_EDITOR
		EditorElement* element = static_cast<EditorElement*>(executedEvent.eventData.at(0));
		EditorScene*   scene   = EditorSceneManager::GetInstance()->GetEditorScene();
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
{
}

EventManager::~EventManager()
{}