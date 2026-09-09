// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "EventSystem/EventFunctions.h"

#include "Utility/TypeDefinitions.h"
#include "Manager/EventManager.h"
#include "Manager/ResourceManager.h"
#include "ResourceSystem/FTPremade.h"
#include "Actor/ActorGroup.h"
#include "Actor/Actor.h"
#include "Actor/Transform.h"
#include "FTMath.h"
#include "EventSystem/EventType.h"

namespace Core
{
	/// @brief Creates an actor instance and schedules it for the requested scene or group.
	/// @param actor Actor participating in this operation.
	/// @param actorGroup Actor group used for scene classification.
	/// @param pos Position or zero-based insertion index.
	/// @return Created instantiate instance or resource.
	Actor* Instantiate(Actor* actor, Common::ActorGroup actorGroup, Math::FTVector3 pos)
	{
		FTEvent addedEvent	= {};
		addedEvent.incident = EVENT_TYPE::CREATE_ACTOR;
		actor->SetActorGroup(actorGroup);

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
	}

	/// @brief Creates an actor instance and schedules it for the requested scene or group.
	/// @param premadeName Registered name of the premade resource.
	/// @return Created instantiate instance or resource.
	Actor* Instantiate(const char* premadeName)
	{
		FTEvent addedEvent		 = {};
		addedEvent.incident		 = EVENT_TYPE::CREATE_ACTOR;
		Core::FTPremade* premade = Core::ResourceManager::GetInstance()->GetPremade(premadeName);
		Actor*			 origin	 = premade->GetOrigin();
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
	}

	/// @brief Releases the managed instance or schedules the specified actor for destruction.
	/// @param actor Actor participating in this operation.
	void Destroy(Actor* actor)
	{
		FTEvent addedEvent	= {};
		addedEvent.incident = EVENT_TYPE::DESTROY_ACTOR;
		// addedEvent.eventData.push_back(actor);
		EventManager::GetInstance()->AddEvent(addedEvent);
	}
} // namespace Core
