#pragma once
#include "Components/ColliderComponent.h"
#include "Actors/Actor.h"
#include "Math/FTMath.h"

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