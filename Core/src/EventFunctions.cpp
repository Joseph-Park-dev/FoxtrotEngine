// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "EventFunctions.h"

#include "TypeDefinitions.h"
#include "Manager/EventManager.h"
#include "Manager/ResourceManager.h"
#include "ResourceSystem/FTPremade.h"
#include "Actor/ActorGroup.h"
#include "Actor/Actor.h"
#include "Math/FTMath.h"
#include "EventType.h"

#ifdef FOXTROT_EDITOR
	#include "FTCoreEditor.h"
	#include "EditorSceneManager.h"
	#include "EditorElement.h"
#endif // FOXTROT_EDITOR

namespace Core
{
	Actor* Instantiate(Actor* actor, ActorGroup actorGroup, Math::FTVector3 pos)
	{
		FTEvent addedEvent	= {};
		addedEvent.incident = EVENT_TYPE::CREATE_ACTOR;
		actor->SetActorGroup(actorGroup);

#ifdef FOXTROT_EDITOR
		EditorScene* scene = EditorSceneManager::GetInstance()->GetEditorScene();
		actor->GetTransform()->SetWorldPosition(pos);
		EditorElement* editorElement = DBG_NEW EditorElement(actor, ChunkKey::ID::CLONE);
		editorElement->Initialize(FTCoreEditor::GetInstance());
		editorElement->Setup();

		if (editorElement)
		{
			addedEvent.eventData.push_back(editorElement);
			addedEvent.eventData.push_back(nullptr);
			EventManager::GetInstance()->AddEvent(addedEvent);
			return editorElement;
		}
		else
		{
			printf("ERROR : Instantiate() -> Premade not loaded, %s\n", actor->GetName().C_Str());
			return nullptr;
		}
#else
		if (actor)
		{
			// actor->Initialize();
			// actor->Setup();

			// addedEvent.eventData.push_back(actor);
			// addedEvent.eventData.push_back(nullptr);
			EventManager::GetInstance()->AddEvent(addedEvent);
			return actor;
		}
		else
		{
			printf("ERROR : Instantiate() -> Premade not loaded, %s\n", actor->GetName().C_Str());
			return nullptr;
		}

#endif
	}

	Actor* Instantiate(const char* premadeName)
	{
		FTEvent addedEvent	= {};
		addedEvent.incident = EVENT_TYPE::CREATE_ACTOR;
		FTPremade* premade	= Core::GET_RES(FTPremade, premadeName);
		Actor*	   origin	= premade->GetOrigin();

#ifdef FOXTROT_EDITOR
		EditorScene*   scene		 = EditorSceneManager::GetInstance()->GetEditorScene();
		EditorElement* editorElement = DBG_NEW EditorElement(origin, ChunkKey::ID::CLONE);
		editorElement->Initialize(FTCoreEditor::GetInstance());
		editorElement->Setup();

		if (editorElement)
		{
			addedEvent.eventData.push_back(editorElement);
			addedEvent.eventData.push_back(nullptr);
			EventManager::GetInstance()->AddEvent(addedEvent);
			return editorElement;
		}
		else
		{
			printf("ERROR : Instantiate() -> Premade not loaded, %s\n", premadeName.C_Str());
			return nullptr;
		}
#else
		if (origin)
		{
			// origin->Initialize(FTCore::GetInstance());
			// origin->Setup();

			// addedEvent.eventData.push_back(origin);
			// addedEvent.eventData.push_back(nullptr);
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
		FTEvent addedEvent	= {};
		addedEvent.incident = EVENT_TYPE::DESTROY_ACTOR;
		// addedEvent.eventData.push_back(actor);
		EventManager::GetInstance()->AddEvent(addedEvent);
	}
} // namespace Core