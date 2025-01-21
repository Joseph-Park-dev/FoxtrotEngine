// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Managers/EventManager.h"

#include "Managers/SceneManager.h"
#include "Managers/UIManager.h"
#include "Core/EventType.h"
#include "Actors/Actor.h"
#include "Actors/ActorGroup.h"
#include "Components/AI.h"
#include "Components/AIState.h"
#include "Scenes/Scene.h"
#include "ResourceSystem/FTPremade.h"

#ifdef FOXTROT_EDITOR
#include "FTCoreEditor.h"
#include "EditorSceneManager.h"
#include "EditorElement.h"
#endif // FOXTROT_EDITOR

void EventManager::ProcessEvent()
{
	for (size_t i = 0; i < mEvent.size(); ++i)
		Execute(mEvent[i]);
	mEvent.clear();
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
		Actor* created = static_cast<Actor*>(executedEvent.eventData.at(0));
		Scene* scene = SceneManager::GetInstance()->GetCurrentScene();
		scene->AddActor(created, created->GetActorGroup());
#endif
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