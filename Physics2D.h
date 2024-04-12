#pragma once
#include "SingletonMacro.h"

class Vector2;
enum class ACTOR_GROUP;
class ColliderComponent;
class Rigidbody2DComponent;

struct PointMass
{
	PointMass()
		: mass(0.f), origin2Point(Vector3::Zero), relativePosition(Vector3::Zero)
	{}
	float	mass;
	Vector3 origin2Point;
	Vector3 relativePosition;
	Vector3 localInertia;
};

struct RayCastHit2D
{
	RayCastHit2D()
		: collider(nullptr), actor(nullptr), distance(0.f), point(Vector2::Zero)
	{}
	ColliderComponent*	  collider;
	Actor*				  actor;
	float				  distance;
	Vector2				  point;
};

struct Ray
{
	Ray(Vector2 orig, Vector2 end, ACTOR_GROUP group)
		:origin(orig), endPoint(end), groupFilter(group)
	{}
	Vector2		origin;
	Vector2		endPoint;
	ACTOR_GROUP groupFilter;
};

class Physics2D
{
	SINGLETON(Physics2D);

public:
	static RayCastHit2D Raycast(Vector2 origin, Vector2 direction, float distance, ACTOR_GROUP actor);
	void RegisterRayCastHit(RayCastHit2D rc) { mRayCastHits.emplace_back(rc); }
	void ResetRayCasts();

public:
	std::vector<RayCastHit2D>& GetRayCastHits(){ return mRayCastHits; }
	void RenderRayCastHits(SDL_Renderer* renderer);
	/*b2World* GetPhysicsWorld() { return mPhysicsWorld; }
	static b2Vec2 ToB2Vec2(Vector2 val) { return b2Vec2(val.x, val.y); }*/

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
	Vector3 CalcCenterOfGravity(int numElements);
	Matrix3 CalcInertiaProduct(int numElements);
};