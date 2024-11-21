#pragma once
#include <string>
#include <list>
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
	int ActorCount;
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
	void SaveActorsData(std::ofstream& out);

protected:
	void LoadActors(std::ifstream& ifs);
	//void LoadActorsToEditor(std::ifstream& ifs);

private:
	ComponentLoadMap mComponentLoadMap;
	ChunkData mCurrentChunkData;

private:
	Actor*		LoadIndividualActor(std::ifstream& ifs, Scene* currScene);
	std::string GetConvertedFileName(std::string curr, std::string prevSuffix, std::string postSuffix);
};

#define STRING_BUFFER_SIZE 50 * sizeof(char)
#define WSTRING_BUFFER_SIZE 50 * sizeof(wchar_t)