// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Managers/CollisionManager.h"

#include <iostream>
#include <unordered_map>

#include "Scenes/Scene.h"
#include "Managers/SceneManager.h"
#include "Actors/Actor.h"
#include "Actors/Transform.h"
#include "Math/FTMath.h"
#include "Physics/Physics2D.h"
#include "Renderer/FoxtrotRenderer.h"
#include "FileSystem/FileIOHelper.h"
#include "FileSystem/ChunkFileKeys.h"

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
	memset(
		mCollisionMarks, 
		0, 
		sizeof(bool) * ActorGroupUtil::GetCount() * ActorGroupUtil::GetCount()
	);
	mRegColliders.clear();
}

void CollisionManager::RegisterCollider(int32_t index, Collider2D* collider)
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

void CollisionManager::UpdateCollisionGroup()
{
	b2ContactEvents events = b2World_GetContactEvents(Physics2D::GetInstance()->GetCurrentWorldID());
	static b2ShapeId shapeIdA;
	static b2ShapeId shapeIdB;

	Collider2D* colliderA = nullptr;
	Collider2D* colliderB = nullptr;

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

void CollisionManager::SaveCollisionMarks(std::ofstream& ofs)
{
	FileIOHelper::BeginDataPackSave(ofs, ChunkKeys::COLLISION_MANAGER);
	FileIOHelper::BeginDataPackSave(ofs, ChunkKeys::COLLISION_MARKS);
	for (size_t row = 0; row < ActorGroupUtil::GetCount(); ++row)
	{
		for (size_t col = 0; col < row + 1; ++col)
		{
			std::string mark =
				std::string(ActorGroupUtil::GetActorGroupStr(row)) + '/'
				+ std::string(ActorGroupUtil::GetActorGroupStr(col));

			size_t idx = ActorGroupUtil::GetCount() * row + col;
			size_t reflectedIdx = ActorGroupUtil::GetCount() * col + row;
			FileIOHelper::SaveBool(ofs, mark, mCollisionMarks[idx]);
		}
	}
	FileIOHelper::EndDataPackSave(ofs, ChunkKeys::COLLISION_MARKS);
	FileIOHelper::EndDataPackSave(ofs, ChunkKeys::COLLISION_MANAGER);
}

void CollisionManager::LoadCollisionMarks(std::ifstream& ifs)
{
	FileIOHelper::BeginDataPackLoad(ifs, ChunkKeys::COLLISION_MANAGER);
	std::pair<size_t, std::string> pack = FileIOHelper::BeginDataPackLoad(ifs, ChunkKeys::COLLISION_MARKS);
	std::vector<bool> marksCache = {};
	marksCache.reserve(pack.first);

	for (size_t i = 0; i < pack.first; ++i)
	{
		bool value = false;
		FileIOHelper::LoadBool(ifs, value);
		marksCache.push_back(value);
	}

	// Linear marks should be reversed because of the order in file.
	// After reversed, FIRST GROUP / FIRST GROUP combination should come first.
	std::reverse(marksCache.begin(), marksCache.end()); 

	for (size_t row = 0; row < ActorGroupUtil::GetCount(); ++row)
	{
		for (size_t col = 0; col < row + 1; ++col)
		{
			size_t idx = ActorGroupUtil::GetCount() * col + row;
			size_t reflectedIdx = ActorGroupUtil::GetCount() * row + col;

			bool mark = marksCache.back();
			mCollisionMarks[idx] = mark;
			mCollisionMarks[reflectedIdx] = mark;

			marksCache.pop_back();
		}
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
		for (size_t row = 0; row < ActorGroupUtil::GetCount(); ++row)
		{
			ImGui::TableNextRow();
			for (size_t col = 0; col < row+1; ++col)
			{
				ImGui::TableSetColumnIndex(col);
				std::string mark =
				std::string(ActorGroupUtil::GetActorGroupStr(row)) + '/'
					+ std::string(ActorGroupUtil::GetActorGroupStr(col));
				size_t idx = ActorGroupUtil::GetCount() * row + col;
				size_t reflectedIdx = ActorGroupUtil::GetCount() * col + row;
				if (ImGui::Checkbox(mark.c_str(), &mCollisionMarks[idx]))
				{
					if (mCollisionMarks[idx]) // if not marked;
					{
						LogInt("marked ", idx);
						mCollisionMarks[reflectedIdx] = true;

						//printf("Marked %s, %s\n",
							//ActorGroupUtil::GetActorGroupStr(i), ActorGroupUtil::GetActorGroupStr(j));
					}
					else
					{
						mCollisionMarks[reflectedIdx] = false;

						printf("Un-Marked %s, %s\n",
							ActorGroupUtil::GetActorGroupStr(row), ActorGroupUtil::GetActorGroupStr(col));
					}
				}
			}
		}
		ImGui::EndTable();
	}
}
#endif // FOXTROT_EDITOR