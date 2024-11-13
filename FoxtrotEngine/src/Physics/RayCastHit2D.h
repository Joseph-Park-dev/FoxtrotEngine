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