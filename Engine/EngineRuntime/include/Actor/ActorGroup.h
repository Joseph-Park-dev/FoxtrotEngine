// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// A tag that sorts the Actors to groups.
/// This is used for making collision marks, or executing a group-based
/// operations.
/// </summary>

#pragma once
#include "TemplateFunctions.h"

namespace Common
{
	enum class ActorGroup
	{
		NOT_ASSIGNED = 0X00000000, // Do not assign this value manually.

		DEFAULT = 0x00000001,
		PLAYER	= 0x00000002,
		ENEMY	= 0x00000003,
		GROUND	= 0x00000004,
		UI		= 0x00000005,

		END
	};

	struct ActorGroupUtil
	{
		/// @brief Returns the count used by this actor group util.
		/// @return Current count.
		static const size_t GetCount()
		{
			return static_cast<size_t>(ActorGroup::END) - 1;
		}

		/// @brief Returns the actor group str used by this actor group util.
		/// @param group Actor group classification.
		/// @return Borrowed access to the actor group str.
		static const char* GetActorGroupStr(ActorGroup group)
		{
			return ActorGroupString[(size_t)group];
		}

		/// @brief Returns the actor group str used by this actor group util.
		/// @param index Zero-based element index.
		/// @return Borrowed access to the actor group str.
		static const char* GetActorGroupStr(size_t index)
		{
			return ActorGroupString.at(index + 1);
		}

		// Index does not guarantee to be ranged within the active elements.
		// For example, index 0 will return NOT_ASSIGNED, which is not used.
		/// @brief Returns the actor group used by this actor group util.
		/// @param index Zero-based element index.
		/// @return Current actor group.
		static ActorGroup GetActorGroup(size_t index)
		{
			return static_cast<ActorGroup>(index);
		}

		/// @brief Returns the actor group used by this actor group util.
		/// @param str Text used by the operation.
		/// @return Current actor group.
		static ActorGroup GetActorGroup(Common::FTDS::String str)
		{
			auto it = std::find(ActorGroupString.begin(), ActorGroupString.end(), str);
			if (it != ActorGroupString.end())
			{
				auto index = it - ActorGroupString.begin();
				return (ActorGroup)index;
			}
			else
				return ActorGroup::DEFAULT;
		}

		static inline std::vector<const char*> ActorGroupString = {
			"NOT_ASSIGNED",
			"DEFAULT",
			"PLAYER",
			"ENEMY",
			"GROUND",
			"UI",
			"END"
		};
	};

	namespace ChunkKey
	{
		constexpr const char* ACTOR_GROUP = "Actor Group";
	}
} // namespace Common
