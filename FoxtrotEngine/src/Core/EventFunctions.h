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
#include <string>

#include "Actors/Actor.h"
#include "Actors/ActorGroup.h"

// Object Spawning & Destroying
Actor*	Instantiate(Actor* actor, ActorGroup actorGroup, FTVector3 pos = FTVector3::Zero);
Actor*	Instantiate(FTDS::String& premadeName);
void	Destroy(Actor* actor);