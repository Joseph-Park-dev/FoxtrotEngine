#include <vector>

#include "Core/FTCore.h"
#include "Managers/KeyInputManager.h"

#include "Managers/CollisionManager.h"
#include "Math/Random.h"
#include "Physics/Physics2D.h"
#include "Physics/Ray.h"
#include "Core/Timer.h"


 // _DEBUG

/* origin : world position vector 
direction : world position vector */
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
	, mWorldID()
	, mSubStepCount(4)
	, mFixedTimeStep(1.f/60.f)
{}

Physics2D::~Physics2D() { b2DestroyWorld(mWorldID); }


RayCastHit2D Physics2D::Raycast(FTVector2 origin, FTVector2 direction, float distance, ActorGroup actor)
{
	FTVector2 rayDir = (direction - origin).Normalize() * distance;
	//Physics::Ray* ray = new Physics::Ray(
	//	//EditorCamera2D::GetInstance()->ConvertWorldPosToScreen(origin),
	//	//EditorCamera2D::GetInstance()->ConvertWorldPosToScreen(origin + rayDir),
	//	actor
	//);
	//CollisionManager::GetInstance()->RegisterRay(ray);
	return RayCastHit2D();
}

//RayCastHit2D Physics2D::Raycast(FTVector2 origin, FTVector2 direction, float distance, ActorGroup actor)
//{
//	FTVector2 rayDir = (direction - origin).Normalize() * distance;
//	Physics::Ray* ray = new Physics::Ray(
//		Camera2D::GetInstance()->ConvertWorldPosToScreen(origin),
//		Camera2D::GetInstance()->ConvertWorldPosToScreen(origin + rayDir),
//		actor
//	);
//	CollisionManager::GetInstance()->RegisterRay(ray);
//	return RayCastHit2D();
//}
