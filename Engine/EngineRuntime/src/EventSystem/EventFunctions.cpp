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
#include "Debugging/DebugMemAlloc.h"

namespace Core
{
	/// @brief Creates an actor instance and schedules it for the requested scene or group.
	/// @param actor Actor participating in this operation.
	/// @param actorGroup Actor group used for scene classification.
	/// @param pos Position or zero-based insertion index.
	/// @return Created instantiate instance or resource.
	Common::IActor* Instantiate(Common::IActor* actor, Common::ActorGroup actorGroup, Math::FTVector3 pos)
	{
		if (!actor)
			return nullptr;

		actor->SetActorGroup(actorGroup);
		if (actor->GetTransform())
			actor->GetTransform()->SetWorldPosition(pos);

		FTEvent addedEvent	 = {};
		addedEvent.incident	 = EVENT_TYPE::CREATE_ACTOR;
		addedEvent.eventData = actor;
		EventManager::GetInstance()->AddEvent(addedEvent);
		return actor;
	}

	/// @brief Creates an actor instance and schedules it for the requested scene or group.
	/// @param premadeName Registered name of the premade resource.
	/// @return Created instantiate instance or resource.
	Common::IActor* Instantiate(const char* premadeName)
	{
		if (!premadeName)
			return nullptr;

		Core::FTPremade* premade = Core::ResourceManager::GetInstance()->GetPremade(premadeName);
		if (!premade || !premade->GetOrigin())
		{
			printf("ERROR : Instantiate() -> Premade not loaded, %s\n", premadeName);
			return nullptr;
		}

		Core::Actor* created = DBG_NEW Actor(premade, Common::ChunkKey::ID::CLONE);

		FTEvent addedEvent	 = {};
		addedEvent.incident	 = EVENT_TYPE::CREATE_ACTOR;
		addedEvent.eventData = created;
		EventManager::GetInstance()->AddEvent(addedEvent);
		return created;
	}

	/// @brief Releases the managed instance or schedules the specified actor for destruction.
	/// @param actor Actor participating in this operation.
	void Destroy(Common::IActor* actor)
	{
		if (!actor)
			return;

		FTEvent addedEvent	 = {};
		addedEvent.incident	 = EVENT_TYPE::DESTROY_ACTOR;
		addedEvent.eventData = actor;
		EventManager::GetInstance()->AddEvent(addedEvent);
	}
} // namespace Core
