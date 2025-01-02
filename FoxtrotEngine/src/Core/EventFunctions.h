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
void	Instantiate(Actor* actor, ActorGroup actorGroup);
Actor*	Instantiate(const char* premadeName);
void	Destroy(Actor* actor);