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
	class FileIOHelper
	{
	public:
		/// @brief Reads the opening .chunk data-pack marker and determines its key and entry count.
		/// @param ifs Input stream positioned at the expected data; reading advances its position.
		/// @return Pair containing the entry count and parsed data-pack key.
		FT_CORE_API static std::pair<size_t, Common::FTDS::String> BeginDataPackLoad(std::ifstream& ifs);
		/// @brief Reads the opening .chunk data-pack marker and determines its key and entry count.
		/// @param ifs Input stream positioned at the expected data; reading advances its position.
		/// @param dataPackKey Key identifying the serialized data-pack section.
		/// @return Pair containing the entry count and parsed data-pack key.
		FT_CORE_API static std::pair<size_t, Common::FTDS::String> BeginDataPackLoad(std::ifstream& ifs, Common::FTDS::String dataPackKey);

		/// @brief Reads the next serialized int value and stores it in the output argument.
		/// @param ifs Input stream positioned at the expected data; reading advances its position.
		/// @param intVal Integer value read, written, or edited.
		/// @note Advances the stream position and updates the destination state.
		FT_CORE_API static void LoadInt(std::ifstream& ifs, int& intVal);
		/// @brief Reads the next serialized unsigned int value and stores it in the output argument.
		/// @param ifs Input stream positioned at the expected data; reading advances its position.
		/// @param intVal Integer value read, written, or edited.
		/// @note Advances the stream position and updates the destination state.
		FT_CORE_API static void LoadUnsignedInt(std::ifstream& ifs, unsigned int& intVal);
		/// @brief Reads the next serialized size value and stores it in the output argument.
		/// @param ifs Input stream positioned at the expected data; reading advances its position.
		/// @param sizeVal Size value read or written.
		/// @note Advances the stream position and updates the destination state.
		FT_CORE_API static void LoadSize(std::ifstream& ifs, size_t& sizeVal);
		/// @brief Reads the next serialized float value and stores it in the output argument.
		/// @param ifs Input stream positioned at the expected data; reading advances its position.
		/// @param floatVal Floating-point value read, written, or edited.
		/// @note Advances the stream position and updates the destination state.
		FT_CORE_API static void LoadFloat(std::ifstream& ifs, float& floatVal);
		/// @brief Reads the next serialized bool value and stores it in the output argument.
		/// @param ifs Input stream positioned at the expected data; reading advances its position.
		/// @param boolVal Boolean value to serialize or convert.
		/// @note Advances the stream position and updates the destination state.
		FT_CORE_API static void LoadBool(std::ifstream& ifs, bool& boolVal);
		/// @brief Reads the next serialized basic string value and stores it in the output argument.
		/// @param ifs Input stream positioned at the expected data; reading advances its position.
		/// @param strVal String value read or written.
		/// @note Advances the stream position and updates the destination state.
		FT_CORE_API static void LoadBasicString(std::ifstream& ifs, Common::FTDS::String& strVal);

		/// @brief Reads the next serialized vector2 value and stores it in the output argument.
		/// @param ifs Input stream positioned at the expected data; reading advances its position.
		/// @param vec2 Two-dimensional vector read or written.
		/// @note Advances the stream position and updates the destination state.
		FT_CORE_API static void LoadVector2(std::ifstream& ifs, Math::FTVector2& vec2);
		/// @brief Reads the next serialized vector3 value and stores it in the output argument.
		/// @param ifs Input stream positioned at the expected data; reading advances its position.
		/// @param vec3 Three-dimensional vector read or written.
		/// @note Advances the stream position and updates the destination state.
		FT_CORE_API static void LoadVector3(std::ifstream& ifs, Math::FTVector3& vec3);
		/// @brief Reads the next serialized vector4 value and stores it in the output argument.
		/// @param ifs Input stream positioned at the expected data; reading advances its position.
		/// @param vec4 Four-dimensional vector read or written.
		/// @note Advances the stream position and updates the destination state.
		FT_CORE_API static void LoadVector4(std::ifstream& ifs, Math::FTVector4& vec4);

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

		/// @brief Extracts a vector3 value from a serialized text line.
		/// @param line Text line to parse or modify.
		/// @param arg Receives the parsed value.
		FT_CORE_API static void ParseVector3(Common::FTDS::String& line, Math::FTVector3& arg);
		/// @brief Extracts a vector2 value from a serialized text line.
		/// @param line Text line to parse or modify.
		/// @param arg Receives the parsed value.
		FT_CORE_API static void ParseVector2(Common::FTDS::String& line, Math::FTVector2& arg);
		/// @brief Extracts a vector4 value from a serialized text line.
		/// @param line Text line to parse or modify.
		/// @param arg Receives the parsed value.
		FT_CORE_API static void ParseVector4(Common::FTDS::String& line, Math::FTVector4& arg);

		/// @brief Extracts a int value from a serialized text line.
		/// @param line Text line to parse or modify.
		/// @param arg Receives the parsed value.
		FT_CORE_API static void ParseInt(Common::FTDS::String& line, int& arg);
		/// @brief Extracts a unsigned int value from a serialized text line.
		/// @param line Text line to parse or modify.
		/// @param arg Receives the parsed value.
		FT_CORE_API static void ParseUnsignedInt(Common::FTDS::String& line, unsigned int& arg);
		/// @brief Extracts a size value from a serialized text line.
		/// @param line Text line to parse or modify.
		/// @param arg Receives the parsed value.
		FT_CORE_API static void ParseSize(Common::FTDS::String& line, size_t& arg);
		/// @brief Extracts a float value from a serialized text line.
		/// @param line Text line to parse or modify.
		/// @param arg Receives the parsed value.
		FT_CORE_API static void ParseFloat(Common::FTDS::String& line, float& arg);
		/// @brief Extracts a bool value from a serialized text line.
		/// @param line Text line to parse or modify.
		/// @param arg Receives the parsed value.
		FT_CORE_API static void ParseBool(Common::FTDS::String& line, bool& arg);
		/// @brief Extracts a string value from a serialized text line.
		/// @param line Text line to parse or modify.
		/// @param arg Receives the parsed value.
		FT_CORE_API static void ParseString(Common::FTDS::String& line, Common::FTDS::String& arg);

	public:
		// Initialize identation, start a new list of item
		/// @brief Writes an opening .chunk data-pack marker for the supplied key.
		/// @param ofs Output stream receiving the serialized data.
		/// @param dataPackKey Key identifying the serialized data-pack section.
		FT_CORE_API static void BeginDataPackSave(std::ofstream& ofs, Common::FTDS::String dataPackKey);
		/// @brief Writes the closing marker for the current .chunk data pack.
		/// @param ofs Output stream receiving the serialized data.
		/// @param dataPackKey Key identifying the serialized data-pack section.
		FT_CORE_API static void EndDataPackSave(std::ofstream& ofs, Common::FTDS::String dataPackKey);
		/// @brief Flushes the accumulated serialization buffer to the output stream.
		/// @param ofs Output stream receiving the serialized data.
		FT_CORE_API static void SaveBufferToFile(std::ofstream& ofs);

		/// @brief Writes a named vector2 value in the engine's .chunk text format.
		/// @param ofs Output stream receiving the serialized data.
		/// @param valName Key written before the serialized value.
		/// @param vec2 Two-dimensional vector read or written.
		/// @note Writes to the supplied stream at its current position.
		FT_CORE_API static void SaveVector2(std::ofstream& ofs, const Common::FTDS::String& valName, const Math::FTVector2& vec2);
		/// @brief Writes a named vector3 value in the engine's .chunk text format.
		/// @param ofs Output stream receiving the serialized data.
		/// @param valName Key written before the serialized value.
		/// @param vec3 Three-dimensional vector read or written.
		/// @note Writes to the supplied stream at its current position.
		FT_CORE_API static void SaveVector3(std::ofstream& ofs, const Common::FTDS::String& valName, const Math::FTVector3& vec3);
		/// @brief Writes a named vector4 value in the engine's .chunk text format.
		/// @param ofs Output stream receiving the serialized data.
		/// @param valName Key written before the serialized value.
		/// @param vec4 Four-dimensional vector read or written.
		/// @note Writes to the supplied stream at its current position.
		FT_CORE_API static void SaveVector4(std::ofstream& ofs, const Common::FTDS::String& valName, const Math::FTVector4& vec4);

		/// @brief Writes a named int value in the engine's .chunk text format.
		/// @param ofs Output stream receiving the serialized data.
		/// @param valName Key written before the serialized value.
		/// @param intVal Integer value read, written, or edited.
		/// @note Writes to the supplied stream at its current position.
		FT_CORE_API static void SaveInt(std::ofstream& ofs, const Common::FTDS::String& valName, const int& intVal);
		/// @brief Writes a named unsigned int value in the engine's .chunk text format.
		/// @param ofs Output stream receiving the serialized data.
		/// @param valName Key written before the serialized value.
		/// @param intVal Integer value read, written, or edited.
		/// @note Writes to the supplied stream at its current position.
		FT_CORE_API static void SaveUnsignedInt(std::ofstream& ofs, const Common::FTDS::String& valName, const unsigned int& intVal);
		/// @brief Writes a named size value in the engine's .chunk text format.
		/// @param ofs Output stream receiving the serialized data.
		/// @param valName Key written before the serialized value.
		/// @param sizeVal Size value read or written.
		/// @note Writes to the supplied stream at its current position.
		FT_CORE_API static void SaveSize(std::ofstream& ofs, const Common::FTDS::String& valName, const size_t& sizeVal);
		/// @brief Writes a named float value in the engine's .chunk text format.
		/// @param ofs Output stream receiving the serialized data.
		/// @param valName Key written before the serialized value.
		/// @param floatVal Floating-point value read, written, or edited.
		/// @note Writes to the supplied stream at its current position.
		FT_CORE_API static void SaveFloat(std::ofstream& ofs, const Common::FTDS::String& valName, const float& floatVal);
		/// @brief Writes a named string value in the engine's .chunk text format.
		/// @param ofs Output stream receiving the serialized data.
		/// @param valName Key written before the serialized value.
		/// @param strVal String value read or written.
		/// @note Writes to the supplied stream at its current position.
		FT_CORE_API static void SaveString(std::ofstream& ofs, Common::FTDS::String& valName, const Common::FTDS::String& strVal);
		/// @brief Writes a named string value in the engine's .chunk text format.
		/// @param ofs Output stream receiving the serialized data.
		/// @param valName Key written before the serialized value.
		/// @param strVal String value read or written.
		/// @note Writes to the supplied stream at its current position.
		FT_CORE_API static void SaveString(std::ofstream& ofs, const char* valName, const Common::FTDS::String& strVal);
		/// @brief Writes a named string value in the engine's .chunk text format.
		/// @param ofs Output stream receiving the serialized data.
		/// @param valName Key written before the serialized value.
		/// @param strVal String value read or written.
		/// @note Writes to the supplied stream at its current position.
		FT_CORE_API static void SaveString(std::ofstream& ofs, const char* valName, const Common::FTDS::String* strVal);
		/// @brief Writes a named string value in the engine's .chunk text format.
		/// @param ofs Output stream receiving the serialized data.
		/// @param valName Key written before the serialized value.
		/// @param strVal String value read or written.
		/// @note Writes to the supplied stream at its current position.
		FT_CORE_API static void SaveString(std::ofstream& ofs, const char* valName, const char* strVal);
		/// @brief Writes a named bool value in the engine's .chunk text format.
		/// @param ofs Output stream receiving the serialized data.
		/// @param valName Key written before the serialized value.
		/// @param boolVal Boolean value to serialize or convert.
		/// @note Writes to the supplied stream at its current position.
		FT_CORE_API static void SaveBool(std::ofstream& ofs, const Common::FTDS::String& valName, const bool& boolVal);

	public:
		/// @brief Returns the line used by this file iohelper.
		/// @param ifs Input stream positioned at the expected data; reading advances its position.
		/// @param str Text used by the operation.
		/// @param delimiter Character that terminates the input line.
		/// @return Current value of the line flag.
		FT_CORE_API static bool GetLine(std::ifstream& ifs, Common::FTDS::String& str, char delimiter = '\n');

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
