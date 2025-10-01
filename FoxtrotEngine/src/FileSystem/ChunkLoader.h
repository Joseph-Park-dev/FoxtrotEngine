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
#include <string>
#include <unordered_map>
#include <functional>
#include <fstream>

#include "Core/SingletonMacro.h"
#include "Math/FTMath.h"
#include "Core/FTCore.h"

#include "Static/HashMap.h"

class Transform;
class Actor;
class Component;
class Scene;

using ComponentLoadFunc = std::function<void(Actor*, std::ifstream&)>;
using ComponentLoadMap	= FTDS::HashMap<ComponentLoadFunc>;

struct ChunkData
{
	size_t ActorCount;
};

class ChunkLoader
{
	SINGLETON_PROTECTED(ChunkLoader)

	// Member Functions for editor level to generate chunk.json files
public:
	virtual void SaveChunk(FTDS::String& fileName);
	virtual void LoadChunk(FTDS::String& fileName);

	void Lock();
	void Unlock();

	void CopyChunk(FTDS::String& path);
	void DeleteCopiedChunk();

public:
	ComponentLoadMap& GetComponentLoadMap() { return mComponentLoadMap; }

	const bool IsLoadingChunk() const;

	FTDS::String& CurrentChunk();

	const int GetMaxActorID() const;
	// Add actor count by 1.
	void AddMaxActorID();

protected:
	// Save .Chunk for the editor
	void SaveChunkData(std::ofstream& out);
	void LoadChunkData(std::ifstream& out);

protected:
	virtual void SaveActorsData(std::ofstream& ofs);
	virtual void LoadActorsData(std::ifstream& ifs);

private:
	ComponentLoadMap mComponentLoadMap;
	ChunkData		 mCurrentChunkData;
	bool			 mIsLoading;

	// Name of the copied .chunk file.
	FTDS::String mCurrentChunkCopy;
	int			 mMaxActorID;
};

namespace ChunkKey
{
	// Data Pack Names
	constexpr const char* CHUNK_DATA	= "Chunk Data";
	constexpr const char* ACTOR_DATA	= "Actor Data";
	constexpr const char* RESOURCE_DATA = "Resource Data";

	constexpr const char* ACTOR_PROPERTIES = "Actor Properties";
	constexpr const char* TRANSFORM		   = "Transform";

	constexpr const char* GAME_DATA		  = "Game Data";
	constexpr const char* CHUNK_LIST	  = "Chunk List";
	constexpr const char* CHUNK_LIST_PATH = "Chunk List Path";

	// Actor Related
	constexpr const char* ACTOR_COUNT = "Actor Count";
} // namespace ChunkKey