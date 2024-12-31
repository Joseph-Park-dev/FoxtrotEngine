// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// A singleton that loads .Chunk to the Foxtrot Editor.
/// .Chunk file, which has a similar purpose with .scene file in UnityEngine,
/// contains all information to organize a scene with various Actors, resources, etc. 
/// 
/// This object contains a map that has ComponentCreateFuncs which will create & assign
/// a new Component to an Actor. You must list all Components to be used into this map.
/// </summary>

#pragma once
#include "FileSystem/ChunkLoader.h"

#include "Core/FTCore.h"

// This creates a Component, initializes and assigns it to an Actor.
using ComponentCreateFunc = std::function<void(Actor*, FTCore*)>;

// You must list all Components to be used during runtime to this map instance.
using ComponentCreateMap = std::unordered_map<std::string, ComponentCreateFunc>;

class EditorChunkLoader :
    public ChunkLoader
{
	SINGLETON(EditorChunkLoader)

public:
	// Saves the current scene on Foxtrot Editor into .Chunk file.
	virtual void SaveChunk(const std::string fileName) override;
	// Loads the a .Chunk file into the current scene on Foxtrot Editor.
	virtual void LoadChunk(const std::string fileName) override;

public:
	ComponentCreateMap& GetCompCreateMap() { return mComponentCreateMap; }

protected:
	// Takes the values from EditorElements to save them into .Chunk file.
	virtual void SaveActorsData(std::ofstream& ofs) override;
	// Reads the values from .Chunk file and assign them to Actors.
	virtual void LoadActorsData(std::ifstream& ifs) override;

private:
	ComponentCreateMap mComponentCreateMap;
};