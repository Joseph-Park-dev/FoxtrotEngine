// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "FileSystem/FileIOHelper.h"

namespace Common
{
	std::list<Common::FTDS::String> Common::FileIOHelper::mDataBuffer	   = {};
	std::list<int>					Common::FileIOHelper::mItemCounts	   = {};
	std::list<Common::FTDS::String> Common::FileIOHelper::mCurrentDataPack = {};
	size_t							Common::FileIOHelper::mDataPackIdent   = 0;
	Common::FTDS::String			Common::FileIOHelper::mItemIdent	   = Common::FTDS::String(mDataPackIdent, '\t');

	// This function is less safe since there is no assert to check data pack name
	/// @brief Reads the opening .chunk data-pack marker and determines its key and entry count.
	/// @param ifs Input stream positioned at the expected data; reading advances its position.
	/// @return Pair containing the entry count and parsed data-pack key.
	std::pair<size_t, Common::FTDS::String> Common::FileIOHelper::BeginDataPackLoad(std::ifstream& ifs)
	{
		Common::FTDS::String loadedDataPackKey;
		GetLine(ifs, loadedDataPackKey, '\n');

		// Parse data pack key
		Common::FTDS::String name;
		loadedDataPackKey.ExtractUntilLast(name, "<");
		if (name.IsEmpty())
			return std::make_pair(0, Common::ChunkKey::NullVal::NULL_OBJECT);
		name.ExtractFromLast("\t");

		// Parse data pack name
		Common::FTDS::String countStr;
		loadedDataPackKey.ExtractBracketedVal(countStr, "<", ">");
		size_t count = std::stoi(countStr.C_Str());

		// std::cout << "Parsing data pack : " << name << '\n';
		// std::cout << name << '\n';

		return std::make_pair(count, name);
	}

	/// @brief Reads the opening .chunk data-pack marker and determines its key and entry count.
	/// @param ifs Input stream positioned at the expected data; reading advances its position.
	/// @param dataPackKey Key identifying the serialized data-pack section.
	/// @return Pair containing the entry count and parsed data-pack key.
	std::pair<size_t, Common::FTDS::String> Common::FileIOHelper::BeginDataPackLoad(std::ifstream& ifs, Common::FTDS::String dataPackKey)
	{
		Common::FTDS::String loadedDataPackKey;
		GetLine(ifs, loadedDataPackKey, '\n');

		// Parse data pack key
		Common::FTDS::String name;
		loadedDataPackKey.ExtractUntilFirst(name, "<");
		name.ExtractFromLast("\t");

		// Parse data pack name
		Common::FTDS::String countStr;
		loadedDataPackKey.ExtractBracketedVal(countStr, "<", ">");
		size_t count = std::stoi(countStr.C_Str());

		if (name != dataPackKey)
		{
			printf("name mismatch!\n");
		}

		assert(name == dataPackKey);

		// std::cout << "Parsing data pack : " << name << '\n';
		// std::cout << name << '\n';

		return std::make_pair(count, name);
	}

	/// @brief Reads the next serialized int value and stores it in the output argument.
	/// @param ifs Input stream positioned at the expected data; reading advances its position.
	/// @param intVal Integer value read, written, or edited.
	/// @note Advances the stream position and updates the destination state.
	void FileIOHelper::LoadInt(std::ifstream& ifs, int& intVal)
	{
		// Parse data information.
		Common::FTDS::String line;
		GetLine(ifs, line, '\n');

		Common::FTDS::String name;
		line.ExtractUntilLast(name, "[");

		Common::FTDS::String typeNameStr;
		line.ExtractBracketedVal(typeNameStr, "[", "]");
		line.Clear();

		// Parse the actual data.
		GetLine(ifs, line, '\n');
		ParseInt(line, intVal);
	}

	/// @brief Reads the next serialized unsigned int value and stores it in the output argument.
	/// @param ifs Input stream positioned at the expected data; reading advances its position.
	/// @param intVal Integer value read, written, or edited.
	/// @note Advances the stream position and updates the destination state.
	void FileIOHelper::LoadUnsignedInt(std::ifstream& ifs, unsigned int& intVal)
	{
		// Parse data information.
		Common::FTDS::String line;
		GetLine(ifs, line, '\n');

		Common::FTDS::String name;
		line.ExtractUntilLast(name, "[");

		Common::FTDS::String typeNameStr;
		line.ExtractBracketedVal(typeNameStr, "[", "]");
		line.Clear();

		// Parse the actual data.
		GetLine(ifs, line, '\n');
		ParseUnsignedInt(line, intVal);
	}

	/// @brief Reads the next serialized size value and stores it in the output argument.
	/// @param ifs Input stream positioned at the expected data; reading advances its position.
	/// @param sizeVal Size value read or written.
	/// @note Advances the stream position and updates the destination state.
	void FileIOHelper::LoadSize(std::ifstream& ifs, size_t& sizeVal)
	{
		// Parse data information.
		Common::FTDS::String line;
		GetLine(ifs, line, '\n');

		Common::FTDS::String name;
		line.ExtractUntilLast(name, "[");

		Common::FTDS::String typeNameStr;
		line.ExtractBracketedVal(typeNameStr, "[", "]");
		line.Clear();

		// Parse the actual data.
		GetLine(ifs, line, '\n');
		ParseSize(line, sizeVal);
	}

	/// @brief Reads the next serialized float value and stores it in the output argument.
	/// @param ifs Input stream positioned at the expected data; reading advances its position.
	/// @param floatVal Floating-point value read, written, or edited.
	/// @note Advances the stream position and updates the destination state.
	void FileIOHelper::LoadFloat(std::ifstream& ifs, float& floatVal)
	{
		// Parse data information.
		Common::FTDS::String line;
		GetLine(ifs, line, '\n');

		Common::FTDS::String name;
		line.ExtractUntilLast(name, "[");

		Common::FTDS::String typeNameStr;
		line.ExtractBracketedVal(typeNameStr, "[", "]");
		line.Clear();

		// Parse the actual data.
		GetLine(ifs, line, '\n');
		ParseFloat(line, floatVal);
	}

	/// @brief Reads the next serialized bool value and stores it in the output argument.
	/// @param ifs Input stream positioned at the expected data; reading advances its position.
	/// @param boolVal Boolean value to serialize or convert.
	/// @note Advances the stream position and updates the destination state.
	void FileIOHelper::LoadBool(std::ifstream& ifs, bool& boolVal)
	{
		// Parse data information.
		Common::FTDS::String line;
		GetLine(ifs, line, '\n');

		Common::FTDS::String name;
		line.ExtractUntilLast(name, "[");

		Common::FTDS::String typeNameStr;
		line.ExtractBracketedVal(typeNameStr, "[", "]");
		line.Clear();

		// Parse the actual data.
		GetLine(ifs, line, '\n');
		ParseBool(line, boolVal);
	}

	/// @brief Reads the next serialized basic string value and stores it in the output argument.
	/// @param ifs Input stream positioned at the expected data; reading advances its position.
	/// @param strVal String value read or written.
	/// @note Advances the stream position and updates the destination state.
	void FileIOHelper::LoadBasicString(std::ifstream& ifs, Common::FTDS::String& strVal)
	{
		// Parse data information.
		Common::FTDS::String line;
		GetLine(ifs, line, '\n');

		Common::FTDS::String name;
		line.ExtractUntilLast(name, "[");

		Common::FTDS::String typeNameStr;
		line.ExtractBracketedVal(typeNameStr, "[", "]");
		line.Clear();

		// Parse the actual data.
		GetLine(ifs, line, '\n');
		ParseString(line, strVal);

		strVal.ExtractFromLast("\t");
	}

	/// @brief Reads the next serialized vector2 value and stores it in the output argument.
	/// @param ifs Input stream positioned at the expected data; reading advances its position.
	/// @param vec2 Two-dimensional vector read or written.
	/// @note Advances the stream position and updates the destination state.
	void FileIOHelper::LoadVector2(std::ifstream& ifs, Math::FTVector2& vec2)
	{
		// Parse data information.
		Common::FTDS::String line;
		GetLine(ifs, line, '\n');

		Common::FTDS::String name;
		line.ExtractUntilLast(name, "[");

		Common::FTDS::String typeNameStr;
		line.ExtractBracketedVal(typeNameStr, "[", "]");
		line.Clear();

		// Parse the actual data.
		GetLine(ifs, line, '\n');
		ParseVector2(line, vec2);
	}

	/// @brief Reads the next serialized vector3 value and stores it in the output argument.
	/// @param ifs Input stream positioned at the expected data; reading advances its position.
	/// @param vec3 Three-dimensional vector read or written.
	/// @note Advances the stream position and updates the destination state.
	void FileIOHelper::LoadVector3(std::ifstream& ifs, Math::FTVector3& vec3)
	{
		// Parse data information.
		Common::FTDS::String line;
		GetLine(ifs, line, '\n');

		Common::FTDS::String name;
		line.ExtractUntilLast(name, "[");

		Common::FTDS::String typeNameStr;
		line.ExtractBracketedVal(typeNameStr, "[", "]");
		line.Clear();

		// Parse the actual data.
		GetLine(ifs, line, '\n');
		ParseVector3(line, vec3);
	}

	/// @brief Reads the next serialized vector4 value and stores it in the output argument.
	/// @param ifs Input stream positioned at the expected data; reading advances its position.
	/// @param vec4 Four-dimensional vector read or written.
	/// @note Advances the stream position and updates the destination state.
	void FileIOHelper::LoadVector4(std::ifstream& ifs, Math::FTVector4& vec4)
	{
		// Parse data information.
		Common::FTDS::String line;
		GetLine(ifs, line, '\n');

		Common::FTDS::String name;
		line.ExtractUntilLast(name, "[");

		Common::FTDS::String typeNameStr;
		line.ExtractBracketedVal(typeNameStr, "[", "]");
		line.Clear();

		// Parse the actual data.
		GetLine(ifs, line, '\n');
		ParseVector4(line, vec4);
	}

	/// @brief Extracts a vector3 value from a serialized text line.
	/// @param line Text line to parse or modify.
	/// @param arg Receives the parsed value.
	void FileIOHelper::ParseVector3(Common::FTDS::String& line, Math::FTVector3& arg)
	{
		line.ExtractBracketedVal(line, "(", ")");

		Common::FTDS::String xStr;
		Common::FTDS::String yStr;
		Common::FTDS::String zStr;

		line.ExtractUntilFirst(xStr, ",");
		line.ExtractBracketedVal(yStr, ",", ",");
		line.ExtractFromLast(zStr, ",");

		float x = std::stof(xStr.C_Str());
		float y = std::stof(yStr.C_Str());
		float z = std::stof(zStr.C_Str());

		arg = Math::FTVector3(x, y, z);
	}

	/// @brief Extracts a vector2 value from a serialized text line.
	/// @param line Text line to parse or modify.
	/// @param arg Receives the parsed value.
	void FileIOHelper::ParseVector2(Common::FTDS::String& line, Math::FTVector2& arg)
	{
		line.ExtractBracketedVal(line, "(", ")");

		Common::FTDS::String xStr;
		Common::FTDS::String yStr;

		line.ExtractUntilFirst(xStr, ",");
		line.ExtractFromLast(yStr, ",");

		float x = std::stof(xStr.C_Str());
		float y = std::stof(yStr.C_Str());

		arg = Math::FTVector2(x, y);
	}

	/// @brief Extracts a vector4 value from a serialized text line.
	/// @param line Text line to parse or modify.
	/// @param arg Receives the parsed value.
	void FileIOHelper::ParseVector4(Common::FTDS::String& line, Math::FTVector4& arg)
	{
		line.ExtractBracketedVal(line, "(", ")");

		// Values at both ends.
		Common::FTDS::String xStr;
		Common::FTDS::String wStr;

		line.ExtractUntilFirst(xStr, ",");
		line.ExtractUntilLast(wStr, ",");

		// Values inbetween others.
		Common::FTDS::String inBetw;
		Common::FTDS::String yStr;
		Common::FTDS::String zStr;

		line.ExtractBracketedVal(inBetw, ",", ",");
		inBetw.ExtractUntilFirst(yStr, ",");
		inBetw.ExtractUntilLast(zStr, ",");

		float x = std::stof(xStr.C_Str());
		float y = std::stof(yStr.C_Str());
		float z = std::stof(zStr.C_Str());
		float w = std::stof(zStr.C_Str());

		arg = Math::FTVector4(x, y, z, w);
	}

	/// @brief Extracts a int value from a serialized text line.
	/// @param line Text line to parse or modify.
	/// @param arg Receives the parsed value.
	void FileIOHelper::ParseInt(Common::FTDS::String& line, int& arg)
	{
		// Remove tab identation.
		size_t tabEnd = line.RFind("\t");
		line.SubStr(tabEnd, line.GetLength());

		arg = std::stoi(line.C_Str());
	}

	/// @brief Extracts a unsigned int value from a serialized text line.
	/// @param line Text line to parse or modify.
	/// @param arg Receives the parsed value.
	void FileIOHelper::ParseUnsignedInt(Common::FTDS::String& line, unsigned int& arg)
	{
		// Remove tab identation.
		size_t tabEnd = line.RFind("\t");
		line.SubStr(tabEnd, line.GetLength());

		arg = std::stoul(line.C_Str(), nullptr, 0);
	}

	/// @brief Extracts a size value from a serialized text line.
	/// @param line Text line to parse or modify.
	/// @param arg Receives the parsed value.
	void FileIOHelper::ParseSize(Common::FTDS::String& line, size_t& arg)
	{
		// Remove tab identation.
		size_t tabEnd = line.RFind("\t");
		line.SubStr(tabEnd, line.GetLength());

		arg = static_cast<size_t>(std::stoul(line.C_Str(), nullptr, 0));
	}

	/// @brief Extracts a float value from a serialized text line.
	/// @param line Text line to parse or modify.
	/// @param arg Receives the parsed value.
	void FileIOHelper::ParseFloat(Common::FTDS::String& line, float& arg)
	{
		// Remove tab identation.
		size_t tabEnd = line.RFind("\t");
		line.SubStr(tabEnd, line.GetLength());

		arg = std::stof(line.C_Str());
	}

	/// @brief Extracts a bool value from a serialized text line.
	/// @param line Text line to parse or modify.
	/// @param arg Receives the parsed value.
	void FileIOHelper::ParseBool(Common::FTDS::String& line, bool& arg)
	{
		// Remove tab identation.
		line.ExtractFromLast("\t");
		arg = StrToBool(line);
	}

	/// @brief Extracts a string value from a serialized text line.
	/// @param line Text line to parse or modify.
	/// @param arg Receives the parsed value.
	void FileIOHelper::ParseString(Common::FTDS::String& line, Common::FTDS::String& arg)
	{
		// Remove tab identation.
		size_t tabEnd = line.RFind("\t");
		line.SubStr(tabEnd, line.GetLength());

		arg.Assign(line);
	}

	/// @brief Writes an opening .chunk data-pack marker for the supplied key.
	/// @param ofs Output stream receiving the serialized data.
	/// @param dataPackKey Key identifying the serialized data-pack section.
	void FileIOHelper::BeginDataPackSave(std::ofstream& ofs, Common::FTDS::String dataPackKey)
	{
		mDataPackIdent = mItemCounts.size();
		mItemIdent	   = Common::FTDS::String(mDataPackIdent + 1, '\t');

		if (0 < mItemCounts.size())
		{
			++mItemCounts.back();
		}
		mItemCounts.push_back(0);
		mCurrentDataPack.push_back(dataPackKey);
	}

	/// @brief Writes the closing marker for the current .chunk data pack.
	/// @param ofs Output stream receiving the serialized data.
	/// @param dataPackKey Key identifying the serialized data-pack section.
	void FileIOHelper::EndDataPackSave(std::ofstream& ofs, Common::FTDS::String dataPackKey)
	{
		assert(mCurrentDataPack.back() == dataPackKey);

		int itemCount = mItemCounts.back();
		mItemCounts.pop_back();
		mCurrentDataPack.pop_back();
		mDataPackIdent = mItemCounts.size();
		mItemIdent	   = Common::FTDS::String(mDataPackIdent + 1, '\t');

		// Returning to the datapack ident level
		Common::FTDS::String title = Common::FTDS::String(mDataPackIdent, '\t') + dataPackKey.C_Str() + "<" + std::to_string(itemCount).c_str() + ">";
		mDataBuffer.push_back(title);
	}

	/// @brief Flushes the accumulated serialization buffer to the output stream.
	/// @param ofs Output stream receiving the serialized data.
	void FileIOHelper::SaveBufferToFile(std::ofstream& ofs)
	{
		// Key & Value pairs = size() * 2
		std::list<Common::FTDS::String>::reverse_iterator iter = mDataBuffer.rbegin();
		for (; iter != mDataBuffer.rend(); ++iter)
		{
			ofs << (*iter).C_Str() << '\n';
		}
		mDataBuffer.clear();
	}

	/// @brief Writes a named vector3 value in the engine's .chunk text format.
	/// @param ofs Output stream receiving the serialized data.
	/// @param valName Key written before the serialized value.
	/// @param vec3 Three-dimensional vector read or written.
	/// @note Writes to the supplied stream at its current position.
	void FileIOHelper::SaveVector3(std::ofstream& ofs, const Common::FTDS::String& valName, const Math::FTVector3& vec3)
	{
		Common::FTDS::String itemTitle = mItemIdent + valName + "[Vector3]" + "\n";
		Common::FTDS::String item	   = mItemIdent +
			"(" + std::to_string(vec3.x).c_str() + "," + std::to_string(vec3.y).c_str() + "," + std::to_string(vec3.z).c_str() + ")";
		mDataBuffer.push_back(itemTitle + item);
		++mItemCounts.back();
	}

	/// @brief Writes a named vector2 value in the engine's .chunk text format.
	/// @param ofs Output stream receiving the serialized data.
	/// @param valName Key written before the serialized value.
	/// @param vec2 Two-dimensional vector read or written.
	/// @note Writes to the supplied stream at its current position.
	void FileIOHelper::SaveVector2(std::ofstream& ofs, const Common::FTDS::String& valName, const Math::FTVector2& vec2)
	{
		Common::FTDS::String itemTitle = mItemIdent + valName + "[Vector2]" + "\n";
		Common::FTDS::String item	   = mItemIdent + "(" + std::to_string(vec2.x).c_str() + "," + std::to_string(vec2.y).c_str() + ")";
		mDataBuffer.push_back(itemTitle + item);
		++mItemCounts.back();
	}

	/// @brief Writes a named vector4 value in the engine's .chunk text format.
	/// @param ofs Output stream receiving the serialized data.
	/// @param valName Key written before the serialized value.
	/// @param vec4 Four-dimensional vector read or written.
	/// @note Writes to the supplied stream at its current position.
	void FileIOHelper::SaveVector4(std::ofstream& ofs, const Common::FTDS::String& valName, const Math::FTVector4& vec4)
	{
		Common::FTDS::String itemTitle = mItemIdent + valName + "[Vector4]" + "\n";
		Common::FTDS::String item =
			mItemIdent +
			"(" +
			std::to_string(vec4.x).c_str() + "," + std::to_string(vec4.y).c_str() +
			std::to_string(vec4.z).c_str() + "," + std::to_string(vec4.w).c_str() +
			")";
		mDataBuffer.push_back(itemTitle + item);
		++mItemCounts.back();
	}

	/// @brief Writes a named int value in the engine's .chunk text format.
	/// @param ofs Output stream receiving the serialized data.
	/// @param valName Key written before the serialized value.
	/// @param intVal Integer value read, written, or edited.
	/// @note Writes to the supplied stream at its current position.
	void FileIOHelper::SaveInt(std::ofstream& ofs, const Common::FTDS::String& valName, const int& intVal)
	{
		Common::FTDS::String itemTitle = mItemIdent + valName + "[int]" + "\n";
		Common::FTDS::String item	   = mItemIdent + std::to_string(intVal).c_str();
		mDataBuffer.push_back(itemTitle + item);
		++mItemCounts.back();
	}

	/// @brief Writes a named unsigned int value in the engine's .chunk text format.
	/// @param ofs Output stream receiving the serialized data.
	/// @param valName Key written before the serialized value.
	/// @param intVal Integer value read, written, or edited.
	/// @note Writes to the supplied stream at its current position.
	void FileIOHelper::SaveUnsignedInt(std::ofstream& ofs, const Common::FTDS::String& valName, const unsigned int& intVal)
	{
		Common::FTDS::String itemTitle = mItemIdent + valName + "[unsigned int]" + "\n";
		Common::FTDS::String item	   = mItemIdent + std::to_string(intVal).c_str();
		mDataBuffer.push_back(itemTitle + item);
		++mItemCounts.back();
	}

	/// @brief Writes a named size value in the engine's .chunk text format.
	/// @param ofs Output stream receiving the serialized data.
	/// @param valName Key written before the serialized value.
	/// @param sizeVal Size value read or written.
	/// @note Writes to the supplied stream at its current position.
	void FileIOHelper::SaveSize(std::ofstream& ofs, const Common::FTDS::String& valName, const size_t& sizeVal)
	{
		Common::FTDS::String itemTitle = mItemIdent + valName + "[size]" + "\n";
		Common::FTDS::String item	   = mItemIdent + std::to_string(sizeVal).c_str();
		mDataBuffer.push_back(itemTitle + item);
		++mItemCounts.back();
	}

	/// @brief Writes a named float value in the engine's .chunk text format.
	/// @param ofs Output stream receiving the serialized data.
	/// @param valName Key written before the serialized value.
	/// @param floatVal Floating-point value read, written, or edited.
	/// @note Writes to the supplied stream at its current position.
	void FileIOHelper::SaveFloat(std::ofstream& ofs, const Common::FTDS::String& valName, const float& floatVal)
	{
		Common::FTDS::String itemTitle = mItemIdent + valName + "[float]" + "\n";
		Common::FTDS::String item	   = mItemIdent + std::to_string(floatVal).c_str();
		mDataBuffer.push_back(itemTitle + item);
		++mItemCounts.back();
	}

	/// @brief Writes a named string value in the engine's .chunk text format.
	/// @param ofs Output stream receiving the serialized data.
	/// @param valName Key written before the serialized value.
	/// @param strVal String value read or written.
	/// @note Writes to the supplied stream at its current position.
	void FileIOHelper::SaveString(std::ofstream& ofs, Common::FTDS::String& valName, const Common::FTDS::String& strVal)
	{
		Common::FTDS::String itemTitle = mItemIdent + valName + "[string]" + "\n";
		Common::FTDS::String item	   = mItemIdent + strVal;
		mDataBuffer.push_back(itemTitle + item);
		++mItemCounts.back();
	}

	/// @brief Writes a named string value in the engine's .chunk text format.
	/// @param ofs Output stream receiving the serialized data.
	/// @param valName Key written before the serialized value.
	/// @param strVal String value read or written.
	/// @note Writes to the supplied stream at its current position.
	void FileIOHelper::SaveString(std::ofstream& ofs, const char* valName, const Common::FTDS::String& strVal)
	{
		Common::FTDS::String itemTitle = mItemIdent + valName + "[string]" + "\n";
		Common::FTDS::String item	   = mItemIdent + strVal.C_Str();
		mDataBuffer.push_back(itemTitle + item);
		++mItemCounts.back();
	}

	/// @brief Writes a named string value in the engine's .chunk text format.
	/// @param ofs Output stream receiving the serialized data.
	/// @param valName Key written before the serialized value.
	/// @param strVal String value read or written.
	/// @note Writes to the supplied stream at its current position.
	void FileIOHelper::SaveString(std::ofstream& ofs, const char* valName, const Common::FTDS::String* strVal)
	{
		Common::FTDS::String itemTitle = mItemIdent + valName + "[string]" + "\n";
		Common::FTDS::String item	   = mItemIdent + strVal->C_Str();
		mDataBuffer.push_back(itemTitle + item);
		++mItemCounts.back();
	}

	/// @brief Writes a named string value in the engine's .chunk text format.
	/// @param ofs Output stream receiving the serialized data.
	/// @param valName Key written before the serialized value.
	/// @param strVal String value read or written.
	/// @note Writes to the supplied stream at its current position.
	void FileIOHelper::SaveString(std::ofstream& ofs, const char* valName, const char* strVal)
	{
		Common::FTDS::String itemTitle = mItemIdent + valName + "[string]" + "\n";
		Common::FTDS::String item	   = mItemIdent + strVal;
		mDataBuffer.push_back(itemTitle + item);
		++mItemCounts.back();
	}

	/// @brief Writes a named bool value in the engine's .chunk text format.
	/// @param ofs Output stream receiving the serialized data.
	/// @param valName Key written before the serialized value.
	/// @param boolVal Boolean value to serialize or convert.
	/// @note Writes to the supplied stream at its current position.
	void FileIOHelper::SaveBool(std::ofstream& ofs, const Common::FTDS::String& valName, const bool& boolVal)
	{
		Common::FTDS::String itemTitle = mItemIdent + valName + "[bool]" + "\n";
		Common::FTDS::String item	   = mItemIdent + ToString(boolVal);
		mDataBuffer.push_back(itemTitle + item);
		++mItemCounts.back();
	}

	/// @brief Returns the line used by this file iohelper.
	/// @param ifs Input stream positioned at the expected data; reading advances its position.
	/// @param str Text used by the operation.
	/// @param delimiter Character that terminates the input line.
	/// @return Current value of the line flag.
	bool FileIOHelper::GetLine(std::ifstream& ifs, Common::FTDS::String& str, char delimiter)
	{
		if (ifs.fail())
			return false;

		char   ch	  = '\0'; // initialize to null character
		size_t length = 0;

		// Calculate the number of characters in a line.
		while (ifs.get(ch))
		{
			if (ch == delimiter)
				break;
			++length;
		}
		int capacity = static_cast<int>(length + 1);
		str.Reserve(capacity);
		ifs.seekg(-capacity - 1, std::ios_base::cur);

		// Read characters one by one.
		for (size_t i = 0; i < length; ++i)
		{
			ifs.get(ch);
			str.PushBack(ch);
		}
		str.PushBack('\0');
		ifs.seekg(2, std::ios_base::cur); // Skip the "\n" at the end of the line.
		return str.Data() && 0 < length;
	}
} // namespace Common
