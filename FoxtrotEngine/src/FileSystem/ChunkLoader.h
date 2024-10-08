#pragma once
#include <string>
#include <unordered_map>
#include <functional>
#include <nlohmann/json.hpp>

#include "Core/SingletonMacro.h"

class FTVector2;
class FTVector3;
class Transform;
class Actor;
class Component;
class Scene;

using ComponentCreateFunc = std::function<void (Actor*)>;
using ComponentLoadFunc = std::function<void(Actor*, std::ifstream& ifs)>;
using ComponentCreateMap = std::unordered_map<std::wstring, ComponentCreateFunc>;
using ComponentLoadMap = std::unordered_map<std::wstring, ComponentLoadFunc>;

#define CHUNK_FILE_FORMAT ".json"
#define FILE_IO_FORMAT ".data"

#define MAPKEY_CHUNKDATA "ChunkData"
#define MAPKEY_ACTORDATA "ActorData"
#define MAPKEY_RESOURCEDATA "ResourceData"

#define SAVEKEY_TARGETACTORID "TargetActorID"
#define SAVEKEY_RENDERRES "RenderResolution"
#define SAVEKEY_SCREENCENTER "ScreenCenter"
#define SAVEKEY_ACTORCOUNT "ActorCount"

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

protected:
	void LoadActors(std::ifstream& ifs);
	//void LoadActorsToEditor(std::ifstream& ifs);

public:
	ComponentCreateMap&	GetCompCreateMap()	{ return mComponentCreateMap; }

private:
	ComponentLoadMap mComponentLoadMap;
	ComponentCreateMap mComponentCreateMap;
	ChunkData mCurrentChunkData;

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

	static FTVector2	LoadVector2		(nlohmann::ordered_json& json, std::string key);
	static FTVector3	LoadVector3		(nlohmann::ordered_json& json, std::string key);

	template <typename ScalarType>
	static ScalarType	LoadScalarValue	(nlohmann::ordered_json& json, std::string key) {
		return json[key];
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