// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// This loads the currently designated .Chunk file for the Scene.
/// A .Chunk file is consist of...
/// 1. Chunk data   : defines the data for the .Chunk itself.
/// 2. Manager data : CollisionMarks from CollisionManager,
///                   FTResources loaded by ResourceManager, etc.
/// 3. Actor data   : Data for the Actors. ActorProperties and
///                   Components assigned to each Actor.
/// 4. Camera Property : TargetActor, Offset, etc.
/// </summary>

#pragma once
#include "FileSystem/IChunkLoader.h"

#include "Utility/SingletonMacro.h"
#include <FTDS/Static/FTString.h>

namespace Core
{
	class ChunkLoader :
		public IChunkLoader
	{
		SINGLETON(ChunkLoader)

		// Member Functions for editor level to generate chunk.json files
	public:
		virtual void SaveChunk(const char* chunkPath) override;
		virtual void LoadChunk(const char* chunkPath) override;

		virtual void Lock() override;
		virtual void Unlock() override;

		virtual void CopyChunk(const char* chunkPath = "./") override;
		virtual void DeleteCopiedChunk() override;

	public:
		// ComponentLoadMap& GetComponentLoadMap() { return mComponentLoadMap; }

		virtual const bool IsLoadingChunk() const override;
		virtual const int  GetMaxActorID() const override;

		// Add actor count by 1.
		virtual void AddMaxActorID() override;
		virtual void ResetMaxActorID() override;

	protected:
		virtual void SaveChunkData(::std::ofstream& out) override;
		virtual void LoadChunkData(::std::ifstream& out) override;

	protected:
		void SaveActorsData(::std::ofstream& ofs);
		void LoadActorsData(::std::ifstream& ifs);

	private:
		ChunkData* mCurrentChunkData;
	};

	namespace ChunkKey
	{
		constexpr const char* GAME_DATA = "Game Data";

		// Data Pack Names
		constexpr const char* CHUNK_DATA = "Chunk Data";
		constexpr const char* ACTOR_DATA = "Actor Data";
		constexpr const char* RES_DATA	 = "Resource Data";

		constexpr const char* ACTOR_PROPERTIES = "Actor Properties";
		constexpr const char* TRANSFORM		   = "Transform";

		constexpr const char* CHUNK_LIST	  = "Chunk List";
		constexpr const char* CHUNK_LIST_PATH = "Chunk List Path";

		// Actor Related
		constexpr const char* ACTOR_COUNT = "Actor Count";
	} // namespace ChunkKey
} // namespace Core