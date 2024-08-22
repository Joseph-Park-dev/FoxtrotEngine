#pragma once
#include <string>
#include <unordered_map>
#include <functional>
#include <nlohmann/json.hpp>

#include "FoxtrotEngine/Core/SingletonMacro.h"

class FTVector2;
class Transform;
class Actor;
class Component;
class Scene;

struct ChunkData
{
	int TargetActorID;
	FTVector2 RenderResolution;
	FTVector2 ScreenCenter;
	size_t ActorCount;
};

using ComponentCreateFunc = std::function<void (Actor*)>;
using ComponentCreateMap = std::unordered_map<std::wstring, ComponentCreateFunc>;
using ComponentLoadFunc = std::function<void(Actor*, std::ifstream& ifs)>;
using ComponentLoadMap = std::unordered_map<std::wstring, ComponentLoadFunc>;

#define CHUNK_FILE_FORMAT ".json"
#define FILE_IO_FORMAT ".data"
class ChunkLoader
{
#define MAPKEY_CHUNKDATA "ChunkData"
#define MAPKEY_ACTORDATA "ActorData"

#define SAVEKEY_TARGETACTORID "TargetActorID"
#define SAVEKEY_RENDERRES "RenderResolution"
#define SAVEKEY_SCREENCENTER "ScreenCenter"
#define SAVEKEY_ACTORCOUNT "ActorCount"


	SINGLETON(ChunkLoader)
// Member Functions for editor level to generate chunk.json files
public:
	void SaveChunk(const std::string fileName);

protected:
	void SaveChunkData(nlohmann::ordered_json& out);
	void SaveResources(std::ofstream& of);
	void SaveActors(nlohmann::ordered_json& out);

// Member functions for engine level to use .chunk files
public:
	void LoadChunk(const std::string fileName);
	void LoadChunkToEditor(const std::string fileName);

protected:
	//void LoadChunkData(YAML::Node& node);
	void LoadActors(std::ifstream& ifs);
	void LoadActorsToEditor(std::ifstream& ifs);

public:
	ComponentCreateMap& GetCompCreateMap()	{ return mComponentCreateMap; }
	ComponentLoadMap&	GetCompLoadMap()	{ return mComponentLoadMap; }

private:
	ChunkData mCurrentChunkData;
	ComponentCreateMap mComponentCreateMap;
	ComponentLoadMap mComponentLoadMap;

private:
	Actor*		LoadIndividualActor(std::ifstream& ifs, Scene* currScene);
	std::string GetConvertedFileName(std::string curr, std::string prevSuffix, std::string postSuffix);
};

#define STRING_BUFFER_SIZE 50 * sizeof(char)
#define WSTRING_BUFFER_SIZE 50 * sizeof(wchar_t)
class FileIOHelper
{
public:
	static void			AddVector2		(nlohmann::ordered_json& json, FTVector2 value);
	static void			AddVector3		(nlohmann::ordered_json& json, FTVector3 value);

	template <typename T>
	static void	AddScalarValue(nlohmann::ordered_json& json, T value) {
		json = value;
	}

public:
	static FTVector2	LoadVector2		(std::ifstream& ifs);
	static std::string	LoadBasicString	(std::ifstream& ifs);
	static std::wstring LoadWString		(std::ifstream& ifs);
	static float		LoadFloat		(std::ifstream& ifs);
	static int			LoadInt			(std::ifstream& ifs);
	static size_t		LoadSize		(std::ifstream& ifs);

	static void			LoadVector2		(std::ifstream& ifs, FTVector2& value);
	static void			LoadBasicString	(std::ifstream& ifs, std::string& value);
	static void			LoadWString		(std::ifstream& ifs, std::wstring& value);
	static void			LoadFloat		(std::ifstream& ifs, float& value);
	static void			LoadInt			(std::ifstream& ifs, int& value);
	static void			LoadSize		(std::ifstream& ifs, size_t& value);

public:
	static void SaveFileIOData(std::string filename);
	static void LoadFileIOData(std::string filename);

public:
	inline static int mUnmatched;
};	

//struct SavePack {
//	int priority = 0;
//};
//
//struct ChunkData : SavePack{
//	int TargetActorID;
//	FTVector2 RenderResolution;
//	FTVector2 ScreenCenter;
//	size_t ActorCount;
//};
//
//struct ActorData : SavePack{
//	std::wstring Name;
//	int			 ID;
//	std::string	 ActorGroup;
//	std::string	 State;
//
//	FTVector2	 WorldPosition;
//	FTVector2	 LocalPosition;
//	float		 Rotation;
//	FTVector2	 Scale;
//
//	
//};
//
//using BasicStringDataPack = std::pair < std::wstring, std::function<void(std::ifstream&)>>;
//using WStringDataPack = std::pair < std::wstring, std::function<void(std::ifstream&)>>;
//using FloatDataPack = std::pair < std::wstring, std::function<void(std::ifstream&)>>;
//using IntegerDataPack = std::pair < std::wstring, std::function<void(std::ifstream&)>>;