#pragma once
#include <string>
#include <unordered_map>

#include "Core/SingletonMacro.h"
#include "Actors/ActorGroup.h"
#include "Physics/Ray.h"

struct Ray;
class ColliderComponent;
class FTVector2;

union COLLIDER_ID
{
	struct
	{
		UINT LeftID;
		UINT RightID;
	};
	uint64_t ID;
};

class CollisionManager
{
	SINGLETON(CollisionManager);

public:
	void MarkGroup(ACTOR_GROUP left, ACTOR_GROUP right);
	void RegisterRay(Physics::Ray* ray);
	void Reset();

public:
	std::vector<Physics::Ray*>& GetRegisteredRays() { return mRegisteredRays; }

private:
	std::unordered_map<uint64_t, bool> mCollisionInfo;
	uint32_t					mArrCollisionMarks[(uint32_t)ACTOR_GROUP::END];
	std::vector<Physics::Ray*>	mRegisteredRays;
	std::vector<FTVector2>		mIntersections;

public:
	void Init() {};
	void Update();
	void RenderRay(FoxtrotRenderer* renderer);


private:
	void UpdateCollisionGroup(ACTOR_GROUP left, ACTOR_GROUP right);
	bool HasCollided(ColliderComponent* leftCol, ColliderComponent* rightCol);
	bool AABBIntersection(ColliderComponent* aCol, ColliderComponent* bCol);
	void ConstainColliderPos(ColliderComponent* aCol, ColliderComponent* bCol);
	void CompareGroupWithRays(ACTOR_GROUP group);
	bool HasRayCasted(Physics::Ray* ray, ColliderComponent* collider, ACTOR_GROUP group);
	bool RayIntersectedWithLine(FTVector2 p1, FTVector2 p2, FTVector2 p3, FTVector2 p4);
	FTVector2 FindClosestIntersection(Physics::Ray* ray);
	void ResetRegisteredRays();
	void ResetIntersections();
};