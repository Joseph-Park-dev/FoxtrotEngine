#pragma once
#include "FileSystem\ChunkLoader.h"

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
	//Save .Chunk for the editor
	void SaveChunkData(nlohmann::ordered_json& ofs);
	void SaveActorsData(nlohmann::ordered_json& ofs);

	void SaveChunkData(std::ofstream& ofs);
	void SaveActorsData(std::ofstream& ofs);

private:
	ComponentCreateMap mComponentCreateMap;
};