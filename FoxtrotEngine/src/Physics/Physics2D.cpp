// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Physics/Physics2D.h"

#include <vector>

#include "Core/FTCore.h"
#include "Managers/KeyInputManager.h"
#include "Math/Random.h"
#include "Physics/Ray.h"
#include "Core/Timer.h"

void Physics2D::ResetRayCasts()
{
	mRayCastHits.clear();
	mRayCastHits.shrink_to_fit();
}

void Physics2D::RenderRayCastHits(FoxtrotRenderer* renderer)
{
}

void Physics2D::Initialize()
{
	float characterHeightInPixels = 64.f;
	float characterHeightInMeters = 1.8f;
	b2SetLengthUnitsPerMeter(characterHeightInPixels / characterHeightInMeters);

	b2WorldDef worldDef = b2DefaultWorldDef();
	worldDef.gravity = b2Vec2{ 0.0f, -9.8f };
	mWorldID = b2CreateWorld(&worldDef);
}

void Physics2D::Update()
{
	b2World_Step(mWorldID, mFixedTimeStep, mSubStepCount);
}

void Physics2D::ShutDown()
{
	b2DestroyWorld(mWorldID);
}

Physics2D::Physics2D()
	: mElements(nullptr)
	, mWorldID()
	, mSubStepCount(4)
	, mFixedTimeStep(1.f/60.f)
{}

Physics2D::~Physics2D() {}

// This will be updated as a function that wraps box2D raycast.
//RayCastHit2D Physics2D::Raycast(FTVector2 origin, FTVector2 direction, float distance, ActorGroup actor)
//{
//	FTVector2 rayDir = (direction - origin).Normalize() * distance;
//	return RayCastHit2D();
//}