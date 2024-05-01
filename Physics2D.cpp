#include <vector>

#include "CCore.h"
#include "Physics2D.h"
#include "KeyInputManager.h"
#include "Camera2D.h"
#include "EditorCamera2D.h"
#include "CollisionManager.h"
#include "Random.h"
#include "Ray.h"

/* origin : world position vector 
direction : world position vector */
#ifdef _DEBUG
RayCastHit2D Physics2D::Raycast(FTVector2 origin, FTVector2 direction, float distance, ACTOR_GROUP actor)
{
	FTVector2 rayDir = (direction - origin).Normalize() * distance;
	Physics::Ray* ray = new Physics::Ray(
		EditorCamera2D::GetInstance()->ConvertWorldPosToScreen(origin), 
		EditorCamera2D::GetInstance()->ConvertWorldPosToScreen(origin + rayDir),
		actor
	);
	CollisionManager::GetInstance()->RegisterRay(ray);
	return RayCastHit2D();
}
#else
RayCastHit2D Physics2D::Raycast(FTVector2 origin, FTVector2 direction, float distance, ACTOR_GROUP actor)
{
	FTVector2 rayDir = (direction - origin).Normalize() * distance;
	Ray* ray = new Ray(
		Camera2D::GetInstance()->ConvertWorldPosToScreen(origin),
		Camera2D::GetInstance()->ConvertWorldPosToScreen(origin + rayDir),
		actor
	);
	CollisionManager::GetInstance()->RegisterRay(ray);
	return RayCastHit2D();
}
#endif // _DEBUG


void Physics2D::ResetRayCasts()
{
	mRayCastHits.clear();
	mRayCastHits.shrink_to_fit();
}

void Physics2D::RenderRayCastHits(FoxtrotRenderer* renderer)
{
	for (size_t i = 0; i < mRayCastHits.size(); ++i)
	{

		/*
		
			Alternative of
				SDL_Rect rect = {};
				rect.x = mRayCastHits[i].point.x;
				rect.y = mRayCastHits[i].point.y;
				rect.w = 10;
				rect.h = 10;
				SDL_RenderDrawRect
				(
					renderer,
					&rect
				);
		
		*/

		
	}
}

void Physics2D::Init()
{
	//b2Vec2 gravity = b2Vec2(0.f, 50.f);
	//mPhysicsWorld = new b2World(gravity);
}

void Physics2D::Update()
{
	//mPhysicsWorld->Step(mTimeStep, mVelocityIterations, mPositionIterations);
}

FTVector3 Physics2D::CalcCenterOfGravity(int numElements)
{
	float totalMass = 0;
	for (size_t i = 0; i < numElements; ++i)
		totalMass += mElements[i].mass;

	FTVector3 firstMoment = FTVector3::Zero;
	for (size_t i = 0; i < numElements; ++i)
		firstMoment += mElements[i].mass * mElements[i].origin2Point;

	FTVector3 centerOfGravity = totalMass / firstMoment;
	for (size_t i = 0; i < numElements; ++i)
		mElements[i].relativePosition = mElements[i].origin2Point - centerOfGravity;
	return centerOfGravity;
}

Matrix3 Physics2D::CalcInertiaProduct(int numElements)
{
	float Ixx, Iyy, Izz, Ixy, Ixz, Iyz;
	Matrix3 inertiaTensor;
	Ixx = 0.f; Iyy = 0.f; Izz = 0.f;
	Ixy = 0.f; Ixz = 0.f; Iyz = 0.f;

	for (int i = 0; i < numElements; ++i)
	{
		Ixx += mElements[i].localInertia.x +
			   mElements[i].mass *
			  (mElements->relativePosition.y * mElements->relativePosition.y +
			   mElements->relativePosition.z * mElements->relativePosition.z);

		Iyy += mElements[i].localInertia.y +
			   mElements[i].mass *
			  (mElements->relativePosition.z * mElements->relativePosition.z +
			   mElements->relativePosition.x * mElements->relativePosition.x);

		Izz += mElements[i].localInertia.z +
			   mElements[i].mass *
			  (mElements->relativePosition.x * mElements->relativePosition.x +
			   mElements->relativePosition.y * mElements->relativePosition.y);

		Ixy += mElements[i].mass * 
			  (mElements[i].relativePosition.x * mElements[i].relativePosition.y);

		Ixz += mElements[i].mass *
			  (mElements[i].relativePosition.x * mElements[i].relativePosition.z);

		Iyz += mElements[i].mass *
			  (mElements[i].relativePosition.y * mElements[i].relativePosition.z);

		inertiaTensor.mat[0][0] =  Ixx;
		inertiaTensor.mat[0][1] = -Ixy;
		inertiaTensor.mat[0][2] = -Ixz;

		inertiaTensor.mat[1][0] = -Ixy;
		inertiaTensor.mat[1][1] =  Iyy;
		inertiaTensor.mat[1][2] = -Iyz;

		inertiaTensor.mat[2][0] = -Ixz;
		inertiaTensor.mat[2][1] = -Iyz;
		inertiaTensor.mat[2][2] =  Izz;

		return inertiaTensor;
	}
}

Physics2D::Physics2D()
	: mElements(nullptr)
	/*, mTimeStep(1.f/60.f)
	, mVelocityIterations(6)
	, mPositionIterations(2)*/
{}

Physics2D::~Physics2D() {}