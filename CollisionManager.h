#pragma once
#include <string>
#include <unordered_map>

#include "SingletonMacro.h"
#include "ActorGroup.h"

struct Ray;
class ColliderComponent;
class Vector2;

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
	void RegisterRay(Ray* ray);
	void Reset();

public:
	std::vector<Ray*>& GetRegisteredRays() { return mRegisteredRays; }

private:
	std::unordered_map<uint64_t, bool> mCollisionInfo;
	uint32_t			 mArrCollisionMarks[(uint32_t)ACTOR_GROUP::END];
	std::vector<Ray*>	 mRegisteredRays;
	std::vector<Vector2> mIntersections;

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
	bool HasRayCasted(Ray* ray, ColliderComponent* collider, ACTOR_GROUP group);
	bool RayIntersectedWithLine(Vector2 p1, Vector2 p2, Vector2 p3, Vector2 p4);
	Vector2 FindClosestIntersection(Ray* ray);
	void ResetRegisteredRays();
	void ResetIntersections();
};

