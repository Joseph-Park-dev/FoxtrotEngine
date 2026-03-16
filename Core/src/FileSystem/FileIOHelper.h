// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Defines helper functionalities to save/load values to/from a
/// .Chunk file.
/// </summary>

#pragma once
#include <filesystem>
#include <list>
#include <DirectXMath.h>

#include "FileSystem/FileTypes.h"
#include "FileSystem/NullKeys.h"
#include "Static/FTString.h"
#include <Static/HashMap.h>

#include <static/Record.h>

class FTVector2;
class FTVector3;
struct b2Vec2;

class FileIOHelper
{
public:
	static std::pair<size_t, FTDS::String> BeginDataPackLoad(std::ifstream& ifs);
	static std::pair<size_t, FTDS::String> BeginDataPackLoad(std::ifstream& ifs, FTDS::String dataPackKey);

	static void LoadInt(std::ifstream& ifs, int& intVal);
	static void LoadUnsignedInt(std::ifstream& ifs, unsigned int& intVal);
	static void LoadSize(std::ifstream& ifs, size_t& sizeVal);
	static void LoadFloat(std::ifstream& ifs, float& floatVal);
	static void LoadBool(std::ifstream& ifs, bool& boolVal);
	static void LoadBasicString(std::ifstream& ifs, FTDS::String& strVal);

	static void LoadVector2(std::ifstream& ifs, FTVector2& vec2);
	static void LoadVector2(std::ifstream& ifs, b2Vec2& vec2);
	static void LoadVector2(std::ifstream& ifs, DirectX::XMFLOAT2& vec2);
	static void LoadVector3(std::ifstream& ifs, FTVector3& vec3);
	static void LoadVector4(std::ifstream& ifs, FTVector4& vec4);
	static void LoadVector4(std::ifstream& ifs, DirectX::XMFLOAT4& vec4);

	template <typename RESOURCE>
	static void LoadResource(std::ifstream& ifs, RESOURCE*& res, FTDS::HashMap<RESOURCE*>* map)
	{
		FTDS::String key;
		LoadBasicString(ifs, key);
		if (key.NotEqual(ChunkKey::NullVal::NULL_OBJECT))
		{
			FTDS::Record<RESOURCE*>* node = map->At(key);
			if (node)
				res = node->Value();
			else
				res = nullptr;
		}
	}

	static void ParseVector3(FTDS::String& line, FTVector3& arg);
	static void ParseVector2(FTDS::String& line, FTVector2& arg);
	static void ParseVector2(FTDS::String& line, b2Vec2& arg);
	static void ParseVector2(FTDS::String& line, DirectX::XMFLOAT2& arg);
	static void ParseVector4(FTDS::String& line, FTVector4& arg);
	static void ParseVector4(FTDS::String& line, DirectX::XMFLOAT4& arg);

	static void ParseInt(FTDS::String& line, int& arg);
	static void ParseUnsignedInt(FTDS::String& line, unsigned int& arg);
	static void ParseSize(FTDS::String& line, size_t& arg);
	static void ParseFloat(FTDS::String& line, float& arg);
	static void ParseBool(FTDS::String& line, bool& arg);
	static void ParseString(FTDS::String& line, FTDS::String& arg);

public:
	// Initialize identation, start a new list of item
	static void BeginDataPackSave(std::ofstream& ofs, FTDS::String dataPackKey);
	static void EndDataPackSave(std::ofstream& ofs, FTDS::String dataPackKey);
	static void SaveBufferToFile(std::ofstream& ofs);

	static void SaveVector3(std::ofstream& ofs, const FTDS::String& valName, const FTVector3& vec3);
	static void SaveVector2(std::ofstream& ofs, const FTDS::String& valName, const FTVector2& vec2);
	static void SaveVector2(std::ofstream& ofs, const FTDS::String& valName, const DirectX::XMFLOAT2& vec2);
	static void SaveVector2(std::ofstream& ofs, const FTDS::String& valName, const b2Vec2& vec2);

	static void SaveVector4(std::ofstream& ofs, const FTDS::String& valName, const FTVector4& vec4);
	static void SaveVector4(std::ofstream& ofs, const FTDS::String& valName, const DirectX::XMFLOAT4& vec4);

	static void SaveInt(std::ofstream& ofs, const FTDS::String& valName, const int& intVal);
	static void SaveUnsignedInt(std::ofstream& ofs, const FTDS::String& valName, const unsigned int& intVal);
	static void SaveSize(std::ofstream& ofs, const FTDS::String& valName, const size_t& sizeVal);
	static void SaveFloat(std::ofstream& ofs, const FTDS::String& valName, const float& floatVal);
	static void SaveString(std::ofstream& ofs, FTDS::String& valName, const FTDS::String strVal);
	static void SaveString(std::ofstream& ofs, FTDS::String& valName, const FTDS::String& strVal);
	static void SaveString(std::ofstream& ofs, const char* valName, const FTDS::String& strVal);
	static void SaveString(std::ofstream& ofs, const char* valName, const char* strVal);
	static void SaveBool(std::ofstream& ofs, const FTDS::String& valName, const bool& boolVal);

public:
	static bool GetLine(std::ifstream& ifs, FTDS::String& str, char delimiter = '\n');

private:
	static std::list<FTDS::String> mDataBuffer;

	// Stores the number of items for each data pack.
	// Pushed back with 0 when nested data pack save is begun
	static std::list<int> mItemCounts;

	// Points to the current data pack that is being saved.
	static std::list<FTDS::String> mCurrentDataPack;
	static size_t				   mDataPackIdent;
	static FTDS::String			   mItemIdent;
};