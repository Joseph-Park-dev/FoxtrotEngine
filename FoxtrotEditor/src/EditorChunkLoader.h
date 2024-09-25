#pragma once
#include "FileSystem\ChunkLoader.h"

class EditorChunkLoader :
    public ChunkLoader
{
	SINGLETON_PROTECTED(EditorChunkLoader)

public:
	virtual void SaveChunk(const std::string fileName) override;
	virtual void LoadChunk(const std::string fileName) override;

protected:
	void SaveChunkData(nlohmann::ordered_json& out);
	void SaveActorsData(nlohmann::ordered_json& out);
};