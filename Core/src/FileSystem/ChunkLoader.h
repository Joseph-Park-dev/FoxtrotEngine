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
#include "Entity/Entity.h"

#include <iosfwd>

#include "SingletonMacro.h"
#include "FTDS/static/FTString.h"

namespace Core
{
	class Transform;
	class Actor;
	class Component;
	class Scene;
	class Plugin;

	struct ChunkData
	{
		size_t ActorCount;
	};

	class ChunkLoader :
		public Core::Entity
	{
		SINGLETON_PROTECTED(ChunkLoader)

		// Member Functions for editor level to generate chunk.json files
	public:
		virtual void SaveChunk(Common::FTDS::String& fileName);
		virtual void LoadChunk(Common::FTDS::String& fileName);

		void Lock();
		void Unlock();

		void CopyChunk(Common::FTDS::String& path);
		void DeleteCopiedChunk();

	public:
		// ComponentLoadMap& GetComponentLoadMap() { return mComponentLoadMap; }

		const bool IsLoadingChunk() const;

		Common::FTDS::String& CurrentChunk();

		const int						GetMaxActorID() const;
		Common::FTDS::HashMap<FARPROC>* GetCompConstructors();

		// Add actor count by 1.
		void AddMaxActorID();

		void ResetMaxActorID();

	protected:
		// Save .Chunk for the editor
		void SaveChunkData(std::ofstream& out);
		void LoadChunkData(std::ifstream& out);

	protected:
		virtual void SaveActorsData(std::ofstream& ofs);
		virtual void LoadActorsData(std::ifstream& ifs);

	private:
		ChunkData mCurrentChunkData;
		bool	  mIsLoading;

		// Name of the copied .chunk file.
		Common::FTDS::String mCurrentChunkCopy;
		int					 mMaxActorID;

		Common::FTDS::HashMap<FARPROC>* mCompConstructors;

	private:
		void LoadPlugins(std::ifstream& ifs);
		void LoadCompConstructors(std::ifstream& ifs, HMODULE& mod);
		void LoadManagerData(std::ifstream& ifs, HMODULE& mod);
	};

	namespace ChunkKey
	{
		// Data Pack Names
		constexpr const char* CHUNK_DATA	= "Chunk Data";
		constexpr const char* ACTOR_DATA	= "Actor Data";
		constexpr const char* CORE_RES_DATA = "Core Resource Data";

		constexpr const char* ACTOR_PROPERTIES = "Actor Properties";
		constexpr const char* TRANSFORM		   = "Transform";

		constexpr const char* CHUNK_LIST	  = "Chunk List";
		constexpr const char* CHUNK_LIST_PATH = "Chunk List Path";

		// Actor Related
		constexpr const char* ACTOR_COUNT = "Actor Count";
	} // namespace ChunkKey
} // namespace Core