#pragma once
#include "Core/SingletonMacro.h"
#include "Physics/RayCastHit2D.h"

class FTVector2;
class Collider2DComponent;
class Rigidbody2DComponent;

#ifdef FOXTROT_EDITOR
#pragma comment (lib, "box2dd.lib")
#endif // FOXTROT_EDITOR

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
	static RayCastHit2D Raycast(FTVector2 origin, FTVector2 direction, float distance, ActorGroup actor);
	void RegisterRayCastHit(RayCastHit2D rc) { mRayCastHits.emplace_back(rc); }
	void ResetRayCasts();

public:
	std::vector<RayCastHit2D>& GetRayCastHits(){ return mRayCastHits; }
	void RenderRayCastHits(FoxtrotRenderer* renderer);
	
	b2WorldId& GetCurrentWorldID() { return mWorldID; }

public:
	void Initialize();
	void Update();

private:
	b2WorldId	mWorldID;
	float		mFixedTimeStep;
	int			mSubStepCount;

	std::vector<RayCastHit2D> mRayCastHits;
	PointMass*				  mElements;

	// These variables kick in when Box2D iss used.
	/*b2World*				  mPhysicsWorld;
	float					  mTimeStep;
	int32					  mVelocityIterations;
	int32					  mPositionIterations;*/

private:
	FTVector3 CalcCenterOfGravity(int numElements);
	Matrix3 CalcInertiaProduct(int numElements);

#ifdef FOXTROT_EDITOR
public:
	void Render();
#endif // FOXTROT_EDITOR

};