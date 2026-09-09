// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
#pragma once
#include <iosfwd>

#include "FTDS/Static/HashMap.h"
#include "FTDS/Static/FTString.h"

namespace Common
{
	class IComponent;
} // namespace Common

namespace Core
{
	struct ChunkData
	{
		Common::FTDS::String					   Path = {};
		Common::FTDS::HashMap<Common::IComponent*> CompLoadMap;
		int										   MaxActorID = 0;
		/// @brief Is this chunk currently being loaded?
		bool IsLoading = false;
	};

	class IChunkLoader
	{
		// Member Functions for editor level to generate chunk.json files
	public:
		/// @brief Serializes the current scene and resources to the selected .chunk file.
		/// @param chunkPath Path to the .chunk file.
		virtual void SaveChunk(const char* chunkPath) = 0;
		/// @brief Restores the scene and its resources from a .chunk file.
		/// @param chunkPath Path to the .chunk file.
		virtual void LoadChunk(const char* chunkPath) = 0;

		/// @brief Halts gameloop while loading a .chunk
		virtual void Lock() = 0;

		/// @brief Resume gameloop after finishing .chunk loading.
		virtual void Unlock() = 0;

		/// @brief To prevent .chunk from corrupting, the copy of the file is made before loading.
		/// The copied .chunk is the one that should be read into the game.
		/// @param path The copy is recommended to be located in the same directory with the original.
		/// @return Full path of the copied .chunk
		/// @param copiedPathOut Receives the path of the temporary chunk copy.
		/// @param chunkPath Path to the .chunk file.
		virtual void CopyChunk(Common::FTDS::String& copiedPathOut, const char* chunkPath = "./") = 0;
		/// @brief Delete the copied chunk after being used.
		virtual void DeleteCopiedChunk() = 0;

	public:
		/// @brief Tests the loading chunk condition for the current object.
		/// @return True if the operation succeeds or the tested condition holds; otherwise false.
		virtual const bool IsLoadingChunk() const = 0;
		/// @brief Returns the max actor id used by this ichunk loader.
		/// @return Current max actor id.
		virtual const int  GetMaxActorID() const  = 0;

		// Add actor count by 1.
		/// @brief Advances the actor identifier counter used during chunk loading.
		virtual void AddMaxActorID()   = 0;
		/// @brief Resets the actor identifier counter before rebuilding scene content.
		virtual void ResetMaxActorID() = 0;

	protected:
		/// @brief Save .chunk specific data
		/// @param out Receives the operation's output.
		virtual void SaveChunkData(::std::ofstream& out) = 0;
		/// @brief Load .chunk specific data
		/// @param out Receives the operation's output.
		virtual void LoadChunkData(::std::ifstream& out) = 0;
	};

	namespace ChunkKey
	{
		constexpr const char* GAME_DATA = "Game Data";

		// Data Pack Names
		constexpr const char* CHUNK_DATA = "Chunk Data";
		constexpr const char* ACTOR_DATA = "Actor Data";
		constexpr const char* ACTOR_COMP = "Actor Component";
		constexpr const char* RES_DATA	 = "Resource Data";

		constexpr const char* TRANSFORM = "Transform";

		constexpr const char* CHUNK_LIST	  = "Chunk List";
		constexpr const char* CHUNK_LIST_PATH = "Chunk List Path";

		// Actor Related
		constexpr const char* ACTOR_COUNT = "Actor Count";
	} // namespace ChunkKey
} // namespace Core
