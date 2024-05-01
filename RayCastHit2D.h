#pragma once
#include "ColliderComponent.h"
#include "Actor.h"
#include "Math.h"

struct RayCastHit2D
{
	RayCastHit2D()
		: collider(nullptr), actor(nullptr), distance(0.f), point(FTVector2::Zero)
	{}
	ColliderComponent*	collider;
	Actor*				actor;
	float				distance;
	FTVector2			point;
};