#pragma once
#include <string>
#include <list>
#include <unordered_map>
#include <functional>
#include <nlohmann/json.hpp>
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
	static std::pair<size_t, std::string> BeginDataPackLoad(std::ifstream& ifs);
	static std::pair<size_t, std::string> BeginDataPackLoad(std::ifstream& ifs, std::string dataPackKey);
	static void   LoadInt			(std::ifstream& ifs, int& intVal);
	static void   LoadUnsignedInt	(std::ifstream& ofs, unsigned int& intVal);
	static void   LoadFloat			(std::ifstream& ifs, float& floatVal);
	static void	  LoadBool			(std::ifstream& ifs, bool& boolVal);
	static void   LoadBasicString	(std::ifstream& ifs, std::string& strVal);
	static void   LoadVector2		(std::ifstream& ifs, FTVector2& vec2);
	static void	  LoadVector2		(std::ifstream& ifs, b2Vec2& vec2);
	static void   LoadVector3		(std::ifstream& ifs, FTVector3& vec3);

	static void	  ParseVector3		(std::string& line, FTVector3& arg);
	static void	  ParseVector2		(std::string& line, FTVector2& arg);
	static void	  ParseVector2		(std::string& line, b2Vec2& arg);

	static void	  ParseInt			(std::string& line, int& arg);
	static void   ParseUnsignedInt	(std::string& line, unsigned int& arg);
	static void	  ParseFloat		(std::string& line, float& arg);
	static void	  ParseBool			(std::string& line, bool& arg);
	static void   ParseString		(std::string& line, std::string& arg);

public:
	// Initialize identation, start new list of item
	static void	BeginDataPackSave	(std::ofstream& ofs, std::string dataPackKey);
	static void	EndDataPackSave		(std::ofstream& ofs, std::string dataPackKey);
	static void SaveBufferToFile	(std::ofstream& ofs);

	static void SaveVector3			(std::ofstream& ofs, const std::string valName, const FTVector3& vec3);
	static void SaveVector2			(std::ofstream& ofs, const std::string valName, const FTVector2& vec2);
	static void SaveVector2			(std::ofstream& ofs, const std::string valName, const b2Vec2& vec2);
	
	static void SaveInt				(std::ofstream& ofs, const std::string valName, const int& intVal);
	static void SaveUnsignedInt		(std::ofstream& ofs, const std::string valName, const unsigned int& intVal);
	static void SaveFloat			(std::ofstream& ofs, const std::string valName, const float& floatVal);
	static void SaveString			(std::ofstream& ofs, const std::string valName, const std::string& strVal);
	static void SaveBool			(std::ofstream& ofs, const std::string valName, const bool& boolVal);

private:
	static std::list<std::string> mDataBuffer;

	// Stores the number of items for each data pack.
	// Pushed back with 0 when nested data pack save is begun
	static std::list<int> mItemCounts;

	// Points to the current data pack that is being saved.
	static std::list<std::string> mCurrentDataPack;
	static int mDataPackIdent;
	static std::string mItemIdent;

private:
	static std::string ExtractUntil(std::string& line, const char end);
	static std::string GetBracketedVal(std::string& str, const char left, const char right);
};	