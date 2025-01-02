// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Stores hit data from a ray.
/// </summary>

#pragma once
#include "Components/Collider2DComponent.h"
#include "Actors/Actor.h"
#include "Math/FTMath.h"

struct RayCastHit2D
{
	RayCastHit2D()
		: collider(nullptr), actor(nullptr), distance(0.f), point(FTVector2::Zero)
	{}
	Collider2DComponent*	collider;
	Actor*				actor;
	float				distance;
	FTVector2			point;
};