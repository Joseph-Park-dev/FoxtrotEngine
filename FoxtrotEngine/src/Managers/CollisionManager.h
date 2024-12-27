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

enum class CollisionState
{
	ENTER,
	STAY,
	EXIT
};

class CollisionManager
{
	SINGLETON(CollisionManager);

public:
	void MarkGroup(b2ShapeDef& object, ActorGroup objectActorGroup);
	void Reset();

public:
	void RegisterCollider(int32_t index, Collider2DComponent* collider);
	b2QueryFilter GetQueryFilter(ActorGroup objectActorGroup);

private:
	bool mCollisionMarks[((size_t)ActorGroup::END-1) * ((size_t)ActorGroup::END-1)];
	std::unordered_map<int32_t, Collider2DComponent*> mRegColliders;

public:
	void Initialize();
	void Update();

private:
	void UpdateCollisionGroup();

#ifdef FOXTROT_EDITOR
public:
	void UpdateUI();

public:
	void UpdateCollisionMarks();
#endif // FOXTROT_EDITOR
};