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
#include "Foxtrot/Core/CoreApi.h"
#include <iosfwd>
#include <list>
#include <fstream>

#include "../TemplateFunctions.h"
#include "NullKeys.h"
#include "../FTDS/Static/FTString.h"
#include "FTMath.h"

namespace Common
{
	class FT_CORE_API FileIOHelper
	{
	public:
		static std::pair<size_t, Common::FTDS::String> BeginDataPackLoad(std::ifstream& ifs);
		static std::pair<size_t, Common::FTDS::String> BeginDataPackLoad(std::ifstream& ifs, Common::FTDS::String dataPackKey);

		static void LoadInt(std::ifstream& ifs, int& intVal);
		static void LoadUnsignedInt(std::ifstream& ifs, unsigned int& intVal);
		static void LoadSize(std::ifstream& ifs, size_t& sizeVal);
		static void LoadFloat(std::ifstream& ifs, float& floatVal);
		static void LoadBool(std::ifstream& ifs, bool& boolVal);
		static void LoadBasicString(std::ifstream& ifs, Common::FTDS::String& strVal);

		static void LoadVector2(std::ifstream& ifs, Math::FTVector2& vec2);
		static void LoadVector3(std::ifstream& ifs, Math::FTVector3& vec3);
		static void LoadVector4(std::ifstream& ifs, Math::FTVector4& vec4);

		// template <typename RESOURCE>
		// static  void LoadResource(std::ifstream& ifs, RESOURCE*& res, Common::FTDS::HashMap<RESOURCE*>* map)
		//{
		//	Common::FTDS::String key;
		//	LoadBasicString(ifs, key);
		//	if (key.NotEqual(Common::ChunkKey::NullVal::NULL_OBJECT))
		//	{
		//		::FTDS::Record<RESOURCE*>* node = map->At(key);
		//		if (node)
		//			res = node->Value();
		//		else
		//			res = nullptr;
		//	}
		// }

		static void ParseVector3(Common::FTDS::String& line, Math::FTVector3& arg);
		static void ParseVector2(Common::FTDS::String& line, Math::FTVector2& arg);
		static void ParseVector4(Common::FTDS::String& line, Math::FTVector4& arg);

		static void ParseInt(Common::FTDS::String& line, int& arg);
		static void ParseUnsignedInt(Common::FTDS::String& line, unsigned int& arg);
		static void ParseSize(Common::FTDS::String& line, size_t& arg);
		static void ParseFloat(Common::FTDS::String& line, float& arg);
		static void ParseBool(Common::FTDS::String& line, bool& arg);
		static void ParseString(Common::FTDS::String& line, Common::FTDS::String& arg);

	public:
		// Initialize identation, start a new list of item
		static void BeginDataPackSave(std::ofstream& ofs, Common::FTDS::String dataPackKey);
		static void EndDataPackSave(std::ofstream& ofs, Common::FTDS::String dataPackKey);
		static void SaveBufferToFile(std::ofstream& ofs);

		static void SaveVector2(std::ofstream& ofs, const Common::FTDS::String& valName, const Math::FTVector2& vec2);
		static void SaveVector3(std::ofstream& ofs, const Common::FTDS::String& valName, const Math::FTVector3& vec3);
		static void SaveVector4(std::ofstream& ofs, const Common::FTDS::String& valName, const Math::FTVector4& vec4);

		static void SaveInt(std::ofstream& ofs, const Common::FTDS::String& valName, const int& intVal);
		static void SaveUnsignedInt(std::ofstream& ofs, const Common::FTDS::String& valName, const unsigned int& intVal);
		static void SaveSize(std::ofstream& ofs, const Common::FTDS::String& valName, const size_t& sizeVal);
		static void SaveFloat(std::ofstream& ofs, const Common::FTDS::String& valName, const float& floatVal);
		static void SaveString(std::ofstream& ofs, Common::FTDS::String& valName, const Common::FTDS::String& strVal);
		static void SaveString(std::ofstream& ofs, const char* valName, const Common::FTDS::String& strVal);
		static void SaveString(std::ofstream& ofs, const char* valName, const Common::FTDS::String* strVal);
		static void SaveString(std::ofstream& ofs, const char* valName, const char* strVal);
		static void SaveBool(std::ofstream& ofs, const Common::FTDS::String& valName, const bool& boolVal);

	public:
		static bool GetLine(std::ifstream& ifs, Common::FTDS::String& str, char delimiter = '\n');

	private:
		static std::list<Common::FTDS::String> mDataBuffer;

		// Stores the number of items for each data pack.
		// Pushed back with 0 when nested data pack save is begun
		static std::list<int> mItemCounts;

		// Points to the current data pack that is being saved.
		static std::list<Common::FTDS::String> mCurrentDataPack;
		static size_t						   mDataPackIdent;
		static Common::FTDS::String			   mItemIdent;

		// static inline void SaveVector2(std::ofstream& ofs, const Common::FTDS::String& valName, const b2Vec2& vec2);
	};
} // namespace Common