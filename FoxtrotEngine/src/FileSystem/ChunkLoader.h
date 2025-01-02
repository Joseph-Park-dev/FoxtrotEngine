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

class Transform;
class Actor;
class Component;
class Scene;

using ComponentLoadFunc = std::function<void(Actor*, std::ifstream&)>;
using ComponentLoadMap = std::unordered_map<std::string, ComponentLoadFunc>;

struct ChunkData
{
	size_t ActorCount;
};

class ChunkLoader
{
	SINGLETON_PROTECTED(ChunkLoader)

// Member Functions for editor level to generate chunk.json files
public:
	virtual void SaveChunk(const std::string fileName);
	virtual void LoadChunk(const std::string fileName);

public:
	ComponentLoadMap& GetComponentLoadMap() { return mComponentLoadMap; }

protected:
	//Save .Chunk for the editor
			void SaveChunkData(std::ofstream& out);
			void LoadChunkData(std::ifstream& out);

protected:
	virtual void SaveActorsData(std::ofstream& ofs);
	virtual void LoadActorsData(std::ifstream& ifs);

private:
	ComponentLoadMap mComponentLoadMap;
	ChunkData mCurrentChunkData;

private:
	std::string GetConvertedFileName(std::string curr, std::string prevSuffix, std::string postSuffix);
};

#define STRING_BUFFER_SIZE 50 * sizeof(char)
#define WSTRING_BUFFER_SIZE 50 * sizeof(wchar_t)