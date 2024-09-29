#pragma once
#include "Core/SingletonMacro.h"
#include "Physics/RayCastHit2D.h"

class FTVector2;
class ColliderComponent;
class Rigidbody2DComponent;

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
	/*b2World* GetPhysicsWorld() { return mPhysicsWorld; }
	static b2Vec2 ToB2Vec2(FTVector2 val) { return b2Vec2(val.x, val.y); }*/

public:
	void Init();
	void Update();

private:
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
};