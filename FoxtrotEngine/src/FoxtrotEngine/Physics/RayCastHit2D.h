#pragma once
#include "FoxtrotEngine/Components/ColliderComponent.h"
#include "FoxtrotEngine/Actors/Actor.h"
#include "FoxtrotEngine/Math/FTMath.h"

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