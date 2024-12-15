#pragma once
#include <string>
#include <unordered_map>

#include "box2d/box2d.h"

#include "Core/SingletonMacro.h"
#include "Actors/ActorGroup.h"
#include "Physics/Ray.h"

struct Ray;
class Collider2DComponent;
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
	void MarkGroup(b2ShapeDef& object, ActorGroup objectActorGroup);
	//void RegisterRay(Physics::Ray* ray);
	void Reset();

public:
	//std::vector<Physics::Ray*>& GetRegisteredRays() { return mRegisteredRays; }
	void RegisterCollider(int32_t index, Collider2DComponent* collider);

private:
	uint64_t mCollisionMarks[(size_t)ActorGroup::END-1];
	std::unordered_map<int32_t, Collider2DComponent*> mRegColliders;

//	std::unordered_map<uint64_t, bool> mCollisionInfo;
//	uint32_t					mArrCollisionMarks[(uint32_t)ActorGroup::END];
//	std::vector<Physics::Ray*>	mRegisteredRays;
//	std::vector<FTVector2>		mIntersections;

public:
	void Initialize();
	void Update();
	//void RenderRay(FoxtrotRenderer* renderer);

private:
	void UpdateCollisionGroup();
	bool IsCollided(ActorGroup groupA, ActorGroup groupB);
	//bool HasCollided(Collider2DComponent* leftCol, Collider2DComponent* rightCol);
	//void UpdateCollisionGroup(ActorGroup left, ActorGroup right);
	//bool AABBIntersection(ColliderComponent* aCol, ColliderComponent* bCol);
	//void ConstainColliderPos(ColliderComponent* aCol, ColliderComponent* bCol);
	//void CompareGroupWithRays(ActorGroup group);
	//bool HasRayCasted(Physics::Ray* ray, ColliderComponent* collider, ActorGroup group);
	//bool RayIntersectedWithLine(FTVector2 p1, FTVector2 p2, FTVector2 p3, FTVector2 p4);
	//FTVector2 FindClosestIntersection(Physics::Ray* ray);
	//void ResetRegisteredRays();
	//void ResetIntersections();

#ifdef FOXTROT_EDITOR
public:
	void UpdateUI();

public:
	void UpdateCollisionMarks();
#endif // FOXTROT_EDITOR
};