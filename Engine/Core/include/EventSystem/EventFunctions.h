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
namespace Math
{
	class FTVector3;
}

namespace Core
{
	class Actor;

	namespace FTDS
	{
		class String;
	}

	// Object Spawning & Destroying
	Actor* Instantiate(Actor* actor, ActorGroup actorGroup, Math::FTVector3 pos);
	Actor* Instantiate(const char* premadeName);
	void   Destroy(Actor* actor);
} // namespace Core