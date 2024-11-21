#pragma once
#include "FileSystem/ChunkLoader.h"

#include "Core/FTCore.h"

using ComponentCreateFunc = std::function<void(Actor*, FTCore*)>;
using ComponentCreateMap = std::unordered_map<std::string, ComponentCreateFunc>;

class EditorChunkLoader :
    public ChunkLoader
{
	SINGLETON(EditorChunkLoader)

public:
	virtual void SaveChunk(const std::string fileName) override;
	virtual void LoadChunk(const std::string fileName) override;

public:
	ComponentCreateMap& GetCompCreateMap() { return mComponentCreateMap; }

protected:
	void SaveChunkData(std::ofstream& ofs);
	void SaveActorsData(std::ofstream& ofs);

	void LoadChunkData(std::ifstream& ifs);
	void LoadActorsData(std::ifstream& ifs);

private:
	ComponentCreateMap mComponentCreateMap;
};