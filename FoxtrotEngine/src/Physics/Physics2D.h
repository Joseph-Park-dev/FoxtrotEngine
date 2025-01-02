// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// A signleton class that manages physics simulation.
/// The simulation is done by Box2D, triggered by the wrapper functions.
/// Features will be added in the future versions.
/// </summary>

#pragma once
#include "Core/SingletonMacro.h"
#include "Physics/RayCastHit2D.h"

class FTVector2;
class Collider2DComponent;
class Rigidbody2DComponent;

#pragma comment (lib, "box2dd.lib")
#include "box2d/box2d.h"

struct PointMass
{
	PointMass()
		: mass(0.f), origin2Point(FTVector3::Zero), relativePosition(FTVector3::Zero)
	{}
	float	  mass;
	FTVector3 origin2Point;
	FTVector3 relativePosition;
	FTVector3 localInertia;
};

class Physics2D
{
	SINGLETON(Physics2D);

public:
	// Raycast wrapper for box2D
	// static RayCastHit2D Raycast(FTVector2 origin, FTVector2 direction, float distance, ActorGroup actor);
	void RegisterRayCastHit(RayCastHit2D rc) { mRayCastHits.emplace_back(rc); }
	void ResetRayCasts();

public:
	std::vector<RayCastHit2D>& GetRayCastHits(){ return mRayCastHits; }
	void RenderRayCastHits(FoxtrotRenderer* renderer);
	
	b2WorldId& GetCurrentWorldID() { return mWorldID; }

public:
	void Initialize();
	void Update();
	void ShutDown();

private:
	b2WorldId	mWorldID;
	float		mFixedTimeStep;
	int			mSubStepCount;

	std::vector<RayCastHit2D> mRayCastHits;
	PointMass*				  mElements;
};