// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Takes care of all collisions occurring in a Scene during a game
/// session. All of them are done by box2D library in the current
/// version.
/// </summary>

#pragma once
#include <string>
#include <unordered_map>

#include "box2d/box2d.h"

#include "Core/Singleton.h"

struct Ray;
class Collider2DComponent;
class FTVector2;
class Collider2D;
enum class ActorGroup;

enum class CollisionState
{
	ENTER,
	STAY,
	EXIT
};

class CollisionManager
{
	SINGLETON(CollisionManager)

public:
	void											MarkGroup(b2ShapeDef& object, ActorGroup objectActorGroup);
	void											Reset();
	const std::unordered_map<int32_t, Collider2D*>& GetRegColliders() const;

public:
	void		  RegisterCollider(int32_t index, Collider2D* collider);
	b2QueryFilter GetQueryFilter(ActorGroup objectActorGroup);
	void		  RemoveCollider(int32_t index);

private:
	bool*									 mCollisionMarks;
	std::unordered_map<int32_t, Collider2D*> mRegColliders;

public:
	void Initialize();
	void Update();

private:
	void UpdateCollisionGroup();

public:
	void SaveCollisionMarks(std::ofstream& ofs);
	void LoadCollisionMarks(std::ifstream& ifs);

#ifdef FOXTROT_EDITOR
public:
	void UpdateUI(bool* opened);

public:
	void UpdateCollisionMarks();
#endif // FOXTROT_EDITOR
};

namespace ChunkKey
{
	constexpr const char* COLLISION_MANAGER = "Collision Manager";
	constexpr const char* COLLISION_MARKS	= "Collision Marks";
} // namespace ChunkKey