// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Core/EventFunctions.h"

#include "Core/TypeDefinitions.h"
#include "Managers/EventManager.h"
#include "Managers/ResourceManager.h"
#include "ResourceSystem/FTPremade.h"

#ifdef FOXTROT_EDITOR
#include "FTCoreEditor.h"
#include "EditorSceneManager.h"
#include "EditorElement.h"
#endif // FOXTROT_EDITOR

void Instantiate(Actor* actor, ActorGroup actorGroup)
{
	FTEvent addedEvent = {};
	addedEvent.incident = EVENT_TYPE::CREATE_ACTOR;
	actor->SetActorGroup(actorGroup);
	addedEvent.eventData.push_back(actor);
	EventManager::GetInstance()->AddEvent(addedEvent);
}

Actor* Instantiate(const char* premadeName)
{
	FTEvent addedEvent = {};
	addedEvent.incident = EVENT_TYPE::CREATE_ACTOR;
	FTPremade* premade = ResourceManager::GetInstance()->GetLoadedPremade(std::move(premadeName));
	Actor* origin = premade->GetOrigin();

#ifdef FOXTROT_EDITOR
	EditorScene* scene = EditorSceneManager::GetInstance()->GetEditorScene();
	EditorElement* editorElement = DBG_NEW EditorElement(origin);
	editorElement->Initialize(FTCoreEditor::GetInstance());

	if (editorElement)
	{
		addedEvent.eventData.push_back(editorElement);
		addedEvent.eventData.push_back(nullptr);
		EventManager::GetInstance()->AddEvent(addedEvent);
		return editorElement;
	}
	else
	{
		printf("ERROR : Instantiate() -> Premade not loaded, %s\n", premadeName);
		return nullptr;
	}
#else
	if (origin)
	{
		addedEvent.eventData.push_back(origin);
		addedEvent.eventData.push_back(nullptr);
		EventManager::GetInstance()->AddEvent(addedEvent);
		return origin;
	}
	else
	{
		printf("ERROR : Instantiate() -> Premade not loaded, %s\n", premadeName);
		return nullptr;
	}
#endif
}

void Destroy(Actor* actor)
{
	FTEvent addedEvent = {};
	addedEvent.incident = EVENT_TYPE::DESTROY_ACTOR;
	addedEvent.eventData.push_back(actor);
	EventManager::GetInstance()->AddEvent(addedEvent);
}