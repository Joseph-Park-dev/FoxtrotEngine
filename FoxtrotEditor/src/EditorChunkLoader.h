#pragma once
#include "FileSystem\ChunkLoader.h"

using ComponentCreateMap = std::unordered_map<std::wstring, ComponentCreateFunc>;

class EditorChunkLoader :
    public ChunkLoader
{
	SINGLETON(EditorChunkLoader)

public:
	virtual void SaveChunk(const std::string fileName) override;
	virtual void LoadChunk(const std::string fileName) override;

protected:
	void SaveChunkData(nlohmann::ordered_json& out);
	void SaveActorsData(nlohmann::ordered_json& out);

private:
	ComponentCreateMap mComponentCreateMap;
};