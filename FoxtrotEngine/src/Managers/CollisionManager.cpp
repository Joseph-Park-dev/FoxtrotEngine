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
	size_t count = ActorGroupUtil::GetCount();
	uint64_t maskBits = 0;
	for (size_t i = 0; i < count; ++i)
	{
		if (mCollisionMarks[((size_t)objectActorGroup-1) * count + i])
			maskBits |= (uint64_t)(i + 1);
		if(mCollisionMarks[i * count + ((size_t)objectActorGroup - 1)])
			maskBits |= (uint64_t)objectActorGroup-1;
	}
	object.filter.maskBits = maskBits;
}

void CollisionManager::Reset()
{
	//memset(mCollisionMarks, 0, sizeof(uint64_t) * ActorGroupUtil::GetCount());
	mRegColliders.clear();
}

void CollisionManager::RegisterCollider(int32_t index, Collider2DComponent* collider)
{
	mRegColliders.insert({index, collider});
}

b2QueryFilter CollisionManager::GetQueryFilter(ActorGroup objectActorGroup)
{
	b2QueryFilter filter = b2DefaultQueryFilter();
	filter.categoryBits = (uint64_t)objectActorGroup;

	size_t count = ActorGroupUtil::GetCount();
	uint64_t maskBits = 0;
	for (size_t i = 0; i < count; ++i)
	{
		if (mCollisionMarks[((size_t)objectActorGroup - 1) * count + i])
			maskBits |= (uint64_t)(i + 1);
		if (mCollisionMarks[i * count + ((size_t)objectActorGroup - 1)])
			maskBits |= (uint64_t)objectActorGroup - 1;
	}
	filter.maskBits = maskBits;
	return filter;
}

CollisionManager::CollisionManager()
	: mCollisionMarks()
	, mRegColliders()
{}

CollisionManager::~CollisionManager()
{}

void CollisionManager::Update()
{
	UpdateCollisionGroup();
}

void CollisionManager::Print()
{
	for (auto& shape : mRegColliders)
		LogString(shape.second->GetOwner()->GetName());
}

void CollisionManager::UpdateCollisionGroup()
{
	b2ContactEvents events = b2World_GetContactEvents(Physics2D::GetInstance()->GetCurrentWorldID());
	static b2ShapeId shapeIdA;
	static b2ShapeId shapeIdB;

	Collider2DComponent* colliderA = nullptr;
	Collider2DComponent* colliderB = nullptr;

	for (size_t i = 0; i < events.beginCount; ++i)
	{
		b2ContactBeginTouchEvent* ev = events.beginEvents + i;
		shapeIdA = ev->shapeIdA;
		shapeIdB = ev->shapeIdB;

		colliderA = mRegColliders.at(shapeIdA.index1);
		colliderB = mRegColliders.at(shapeIdB.index1);

		colliderA->GetCollisionStates().insert({ shapeIdB.index1, CollisionState(CollisionState::ENTER) });
		colliderB->GetCollisionStates().insert({ shapeIdA.index1, CollisionState(CollisionState::ENTER) });

		colliderA->OnCollisionEnter(colliderB);
		colliderB->OnCollisionEnter(colliderA);

		colliderA->GetCollisionStates()[shapeIdB.index1] = CollisionState::STAY;
		colliderB->GetCollisionStates()[shapeIdA.index1] = CollisionState::STAY;
	}

	if (b2Shape_IsValid(shapeIdA) && b2Shape_IsValid(shapeIdB))
	{
		b2ContactData data;
		b2Shape_GetContactData(shapeIdA, &data, 1);

		if (b2Shape_IsValid(data.shapeIdA) && b2Shape_IsValid(data.shapeIdB))
		{
			shapeIdA = data.shapeIdA;
			shapeIdB = data.shapeIdB;

			colliderA = mRegColliders.at(shapeIdA.index1);
			colliderB = mRegColliders.at(shapeIdB.index1);

			if (colliderA->GetCollisionStates()[shapeIdB.index1] == CollisionState::STAY
				&& colliderB->GetCollisionStates()[shapeIdA.index1] == CollisionState::STAY)
			{
				colliderA->OnCollisionStay(colliderB);
				colliderB->OnCollisionStay(colliderA);
			}
		}
	}

	for (size_t i = 0; i < events.hitCount; ++i)
	{
		b2ContactHitEvent* ev = events.hitEvents + i;
		shapeIdA = ev->shapeIdA;
		shapeIdB = ev->shapeIdB;

		colliderA = mRegColliders.at(shapeIdA.index1);
		colliderB = mRegColliders.at(shapeIdB.index1);

		colliderA->OnCollisionStay(colliderB);
		colliderB->OnCollisionStay(colliderA);
	}

	for (size_t i = 0; i < events.endCount; ++i)
	{
		b2ContactEndTouchEvent* ev = events.endEvents + i;
		shapeIdA = ev->shapeIdA;
		shapeIdB = ev->shapeIdB;

		colliderA = mRegColliders.at(shapeIdA.index1);
		colliderB = mRegColliders.at(shapeIdB.index1);

		colliderA->GetCollisionStates()[shapeIdB.index1] = CollisionState::EXIT;
		colliderB->GetCollisionStates()[shapeIdA.index1] = CollisionState::EXIT;

		colliderA->OnCollisionExit(colliderB);
		colliderB->OnCollisionExit(colliderA);

		colliderA->GetCollisionStates().erase(shapeIdB.index1);
		colliderB->GetCollisionStates().erase(shapeIdA.index1);
	}
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
				size_t idx = ActorGroupUtil::GetCount() * i + j;
				if (ImGui::Checkbox(mark.c_str(), &marks[idx]))
				{
					if (marks[idx]) // if not marked;
					{
						mCollisionMarks[idx] = true;
						mCollisionMarks[ActorGroupUtil::GetCount() * j + i] = true;

						printf("Marked %s, %s\n",
							ActorGroupUtil::GetActorGroupStr(i), ActorGroupUtil::GetActorGroupStr(j));
					}
					else
					{
						mCollisionMarks[idx] = false;
						mCollisionMarks[ActorGroupUtil::GetCount() * j + i] = false;

						printf("Un-Marked %s, %s\n",
							ActorGroupUtil::GetActorGroupStr(i), ActorGroupUtil::GetActorGroupStr(j));
					}
				}
			}
		}
		ImGui::EndTable();
	}
}
#endif // FOXTROT_EDITOR