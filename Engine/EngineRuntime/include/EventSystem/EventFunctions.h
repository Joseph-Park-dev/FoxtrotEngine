// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Defines global functions that enqueue the events to the EventManager.
/// </summary>

#pragma once
#include "Plugin/CoreExports.h"

namespace Math
{
	class FTVector3;
}

namespace Common
{
	class IActor;
	enum class ActorGroup;
}

namespace Core
{
	namespace FTDS
	{
		class String;
	}

	// Object Spawning & Destroying
	/// @brief Creates an actor instance and schedules it for the requested scene or group.
	/// @param actor Actor participating in this operation.
	/// @param actorGroup Actor group used for scene classification.
	/// @param pos Position or zero-based insertion index.
	/// @return Created instantiate instance or resource.
	CORE_API Common::IActor* Instantiate(Common::IActor* actor, Common::ActorGroup actorGroup, Math::FTVector3 pos);
	/// @brief Creates an actor instance and schedules it for the requested scene or group.
	/// @param premadeName Registered name of the premade resource.
	/// @return Created instantiate instance or resource.
	CORE_API Common::IActor* Instantiate(const char* premadeName);
	/// @brief Provides an empty lifecycle or extension hook for this implementation.
	/// @param actor Actor participating in this operation.
	CORE_API void Destroy(Common::IActor* actor);
} // namespace Core
