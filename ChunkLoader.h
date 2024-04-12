#pragma once
#include <string>
#include <unordered_map>
#include <functional>

#include "SingletonMacro.h"

class Vector2;
class Transform;
class Actor;
class Component;
class Scene;

struct ChunkData
{
	int ActorCount = 0;
};

using ComponentCreateFunc = std::function<void (Actor*)>;
using ComponentCreateMap = std::unordered_map<std::wstring, ComponentCreateFunc>;
using ComponentLoadFunc = std::function<void(Actor*, std::ifstream& ifs)>;
using ComponentLoadMap = std::unordered_map<std::wstring, ComponentLoadFunc>;

#define CHUNK_FORMAT ".chunk"
#define FILE_IO_FORMAT ".data"
class ChunkLoader
{
	SINGLETON(ChunkLoader)
// Member Functions for editor level to generate chunk.json files
public:
	void SaveChunk(const std::string fileName);

protected:
	void SaveActors(std::ofstream& of);
	void SaveChunkData(std::ofstream& of);


// Member functions for engine level to use .chunk files
public:
	void LoadChunk(const std::string fileName);
	void LoadChunkToEditor(const std::string fileName);

protected:
	void LoadChunkData(std::ifstream& ifs);
	void LoadActors(std::ifstream& ifs);
	void LoadActorsToEditor(std::ifstream& ifs);

public:
	ComponentCreateMap& GetCompCreateMap()	{ return mComponentCreateMap; }
	ComponentLoadMap&	GetCompLoadMap()	{ return mComponentLoadMap; }

private:
	ChunkData mCurrentChunk;
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
	static void			AddVector2		(std::ofstream& ofs, Vector2 value);
	static void			AddBasicString	(std::ofstream& ofs, std::string value);
	static void			AddWString		(std::ofstream& ofs, std::wstring value);
	static void			AddFloat		(std::ofstream& ofs, float value);
	static void			AddInt			(std::ofstream& ofs, int value);
	static void			AddSize			(std::ofstream& ofs, size_t value);

	static Vector2		LoadVector2		(std::ifstream& ifs);
	static std::string	LoadBasicString	(std::ifstream& ifs);
	static std::wstring LoadWString		(std::ifstream& ifs);
	static float		LoadFloat		(std::ifstream& ifs);
	static int			LoadInt			(std::ifstream& ifs);
	static size_t		LoadSize		(std::ifstream& ifs);

	static void			LoadVector2		(std::ifstream& ifs, Vector2& value);
	static void			LoadBasicString	(std::ifstream& ifs, std::string& value);
	static void			LoadWString		(std::ifstream& ifs, std::wstring& value);
	static void			LoadFloat		(std::ifstream& ifs, float& value);
	static void			LoadInt			(std::ifstream& ifs, int& value);
	static void			LoadSize		(std::ifstream& ifs, size_t& value);


public:
	static void SaveFileIOData(std::string filename);
	static void LoadFileIOData(std::string filename);
	static void ResetFileIOData() { mUnmatched = 0; }
	static int	GetUnmatched()	  { return mUnmatched; }

private:
	inline static int mUnmatched;
};