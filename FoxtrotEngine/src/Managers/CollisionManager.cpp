#include "Managers/CollisionManager.h"

#include <iostream>
#include <unordered_map>
#include <bitset>

#include "Scenes/Scene.h"
#include "Managers/SceneManager.h"
#include "Actors/Actor.h"
#include "Actors/Transform.h"
#include "Math/FTMath.h"
#include "Physics/Physics2D.h"
#include "Renderer/FoxtrotRenderer.h"

void CollisionManager::MarkGroup(b2ShapeDef& object, ActorGroup objectActorGroup)
{
	object.filter.maskBits = mCollisionMarks[(uint64_t)objectActorGroup - 1];
}

void CollisionManager::Reset()
{
	memset(mCollisionMarks, 0, sizeof(UINT) * ActorGroupUtil::GetCount());
	mRegColliders.clear();
}

void CollisionManager::RegisterCollider(int32_t index, Collider2DComponent* collider)
{
	mRegColliders.emplace(index, collider);
}

CollisionManager::CollisionManager()
	: mCollisionMarks()
{}

CollisionManager::~CollisionManager()
{
	delete[] mCollisionMarks;
}

void CollisionManager::Update()
{
	UpdateCollisionGroup();
}

void CollisionManager::UpdateCollisionGroup()
{
	b2ContactEvents events = b2World_GetContactEvents(Physics2D::GetInstance()->GetCurrentWorldID());
	for (size_t i = 0; i < events.beginCount; ++i)
	{
		b2ContactBeginTouchEvent* ev = events.beginEvents + i;
		b2ShapeId shapeIdA = ev->shapeIdA;
		b2ShapeId shapeIdB = ev->shapeIdB;

		Collider2DComponent* colliderA = mRegColliders.at(shapeIdA.index1);
		Collider2DComponent* colliderB = mRegColliders.at(shapeIdB.index1);
		
		colliderA->OnCollisionEnter(colliderB);
		colliderB->OnCollisionEnter(colliderA);
	}

	for (size_t i = 0; i < events.hitCount; ++i)
	{
		b2ContactHitEvent* ev = events.hitEvents + i;
		b2ShapeId shapeIdA = ev->shapeIdA;
		b2ShapeId shapeIdB = ev->shapeIdB;

		Collider2DComponent* colliderA = mRegColliders.at(shapeIdA.index1);
		Collider2DComponent* colliderB = mRegColliders.at(shapeIdB.index1);

		colliderA->OnCollisionStay(colliderB);
		colliderB->OnCollisionStay(colliderA);
	}

	for (size_t i = 0; i < events.endCount; ++i)
	{
		b2ContactEndTouchEvent* ev = events.endEvents + i;
		b2ShapeId shapeIdA = ev->shapeIdA;
		b2ShapeId shapeIdB = ev->shapeIdB;

		Collider2DComponent* colliderA = mRegColliders.at(shapeIdA.index1);
		Collider2DComponent* colliderB = mRegColliders.at(shapeIdB.index1);

		colliderA->OnCollisionExit(colliderB);
		colliderB->OnCollisionExit(colliderA);
	}
}

bool CollisionManager::IsCollided(ActorGroup groupA, ActorGroup groupB)
{
	return mCollisionMarks[(uint64_t)groupA] & ((uint64_t)1 << (uint64_t)groupB);
}

void CollisionManager::Initialize()
{}

#ifdef FOXTROT_EDITOR
void CollisionManager::UpdateUI()
{
	UpdateCollisionMarks();
}

void CollisionManager::UpdateCollisionMarks()
{
	if (ImGui::BeginTable("Collision Marks", (int)ActorGroup::END))
	{
		// Since it is needed to make the count constant,
		// ActorGroupUtil::GetCount() is not used.
		const size_t group = (size_t)ActorGroup::END - 1;  
		static bool marks[(group * (group + 1)) / 2];
		for (size_t i = 0; i < ActorGroupUtil::GetCount(); ++i)
		{
			ImGui::TableNextRow();
			for (size_t j = 0; j < i+1; ++j)
			{
				ImGui::TableSetColumnIndex(j);
				std::string mark =
				std::string(ActorGroupUtil::GetActorGroupStr(i)) + '/'
					+ std::string(ActorGroupUtil::GetActorGroupStr(j));
				size_t idx = (size_t)ActorGroup::END * j + i;
				if (ImGui::Checkbox(mark.c_str(), &marks[idx]))
				{
					uint64_t bitmask = static_cast<uint64_t>(j);
					if (!(mCollisionMarks[i] & ((uint64_t)1 << bitmask))) // if not marked;
					{
						mCollisionMarks[i] |= ((uint64_t)1 << bitmask);
						printf("Marked %s, %s\n",
							ActorGroupUtil::GetActorGroupStr(i), ActorGroupUtil::GetActorGroupStr(j));
					}
					else
					{
						printf("Un-Marked %s, %s\n",
							ActorGroupUtil::GetActorGroupStr(i), ActorGroupUtil::GetActorGroupStr(j));
						mCollisionMarks[i] &= ~((uint64_t)1 << bitmask);
					}
				}
			}
		}
		ImGui::EndTable();
	}
}
#endif // FOXTROT_EDITOR