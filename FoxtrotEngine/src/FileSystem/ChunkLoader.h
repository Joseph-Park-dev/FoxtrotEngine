#pragma once
#include <string>
#include <unordered_map>
#include <functional>
#include <nlohmann/json.hpp>
#include <fstream>

#include "Core/SingletonMacro.h"
#include "Math/FTMath.h"

class Transform;
class Actor;
class Component;
class Scene;

using ComponentCreateFunc = std::function<void (Actor*)>;
using ComponentLoadFunc = std::function<void(Actor*, std::ifstream& ifs)>;
using ComponentLoadMap = std::unordered_map<std::string, ComponentLoadFunc>;

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
	//Save .Chunk for the editor
	void SaveChunkData(std::fstream& out);
	void SaveActorsData(std::fstream& out);

protected:
	void LoadActors(std::ifstream& ifs);
	void LoadActors(nlohmann::ordered_json& in);
	//void LoadActorsToEditor(std::ifstream& ifs);

private:
	ComponentLoadMap mComponentLoadMap;
	ChunkData mCurrentChunkData;

private:
	Actor*		LoadIndividualActor(std::ifstream& ifs, Scene* currScene);
	Actor*		LoadIndividualActor(nlohmann::ordered_json& ifs, Scene* currScene);
	std::string GetConvertedFileName(std::string curr, std::string prevSuffix, std::string postSuffix);
};

#define STRING_BUFFER_SIZE 50 * sizeof(char)
#define WSTRING_BUFFER_SIZE 50 * sizeof(wchar_t)

std::ofstream& operator<<(std::ofstream& ofs, FTVector3& vec3) {
	ofs << "(" << std::to_string(vec3.x) << "," << std::to_string(vec3.y) << "," << std::to_string(vec3.z) << ")";
	return ofs;
}

std::ofstream& operator<<(std::ofstream& ofs, FTVector2& vec2) {
	ofs << "(" << std::to_string(vec2.x) << "," << std::to_string(vec2.y) << ")";
	return ofs;
}

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
	static int	BeginDataPackLoad(std::ifstream& ifs);

	static void	ParseVector3(std::string& line, FTVector3& arg);
	static void	ParseVector2(std::string& line, FTVector2& arg);

	static void	ParseInt(std::string& line, int& arg);
	static void	ParseFloat(std::string& line, float& arg);
	static void ParseString(std::string& line, std::string& arg);

public:
	static int	BeginDataPackSave(std::ofstream& ofs, const std::string dataPackKey, const int varCount);
	static void SaveVector3(std::ofstream& ofs, const std::string valName, const FTVector3& vec3);
	static void SaveVector2(std::ofstream& ofs, const std::string valName, const FTVector2& vec2);
	
	static void SaveInt(std::ofstream& ofs, const std::string valName, const int& intVal);
	static void SaveFloat(std::ofstream& ofs, const std::string valName, const float& floatVal);
	static void SaveString(std::ofstream& ofs, const std::string valName, const std::string& strVal);

	friend std::ofstream& operator<<(std::ofstream& ofs, FTVector3& vec3);
	friend std::ofstream& operator<<(std::ofstream& ofs, FTVector2& vec2);

private:
	static std::string ExtractUntil(std::string& line, const char end);
	static std::string GetBracketedVal(std::string& str, const char left, const char right);

	private 
};	