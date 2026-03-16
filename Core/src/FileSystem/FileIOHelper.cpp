// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "FileSystem/FileIOHelper.h"

#include <string>
#include <fstream>
#include <iostream>

#include "Core/TemplateFunctions.h"

std::list<FTDS::String> FileIOHelper::mDataBuffer	   = {};
std::list<int>			FileIOHelper::mItemCounts	   = {};
std::list<FTDS::String> FileIOHelper::mCurrentDataPack = {};
size_t					FileIOHelper::mDataPackIdent   = 0;
FTDS::String			FileIOHelper::mItemIdent	   = FTDS::String(mDataPackIdent, '\t');

// This function is less safe since there is no assert to check data pack name
std::pair<size_t, FTDS::String> FileIOHelper::BeginDataPackLoad(std::ifstream& ifs)
{
	FTDS::String loadedDataPackKey;
	GetLine(ifs, loadedDataPackKey, '\n');

	// Parse data pack key
	FTDS::String name;
	loadedDataPackKey.ExtractUntilLast(name, "<");
	if (name.IsEmpty())
		return std::make_pair(0, ChunkKey::NullVal::NULL_OBJECT);
	name.ExtractFromLast("\t");

	// Parse data pack name
	FTDS::String countStr;
	loadedDataPackKey.ExtractBracketedVal(countStr, "<", ">");
	size_t count = std::stoi(countStr.C_Str());

	// std::cout << "Parsing data pack : " << name << '\n';
	// std::cout << name << '\n';

	return std::make_pair( count, name );
}

std::pair<size_t, FTDS::String> FileIOHelper::BeginDataPackLoad(std::ifstream& ifs, FTDS::String dataPackKey)
{
	FTDS::String loadedDataPackKey;
	GetLine(ifs, loadedDataPackKey, '\n');

	// Parse data pack key
	FTDS::String name;
	loadedDataPackKey.ExtractUntilFirst(name, "<");
	name.ExtractFromLast("\t");

	// Parse data pack name
	FTDS::String countStr;
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

void FileIOHelper::LoadInt(std::ifstream& ifs, int& intVal)
{
	// Parse data information.
	FTDS::String line;
	GetLine(ifs, line, '\n');

	FTDS::String name; 
	line.ExtractUntilLast(name, "[");

	FTDS::String typeNameStr; 
	line.ExtractBracketedVal(typeNameStr, "[", "]");
	line.Clear();

	// Parse the actual data.
	GetLine(ifs, line, '\n');
	ParseInt(line, intVal);
}

void FileIOHelper::LoadUnsignedInt(std::ifstream& ifs, unsigned int& intVal)
{
	// Parse data information.
	FTDS::String line;
	GetLine(ifs, line, '\n');

	FTDS::String name;
	line.ExtractUntilLast(name, "[");

	FTDS::String typeNameStr;
	line.ExtractBracketedVal(typeNameStr, "[", "]");
	line.Clear();

	// Parse the actual data.
	GetLine(ifs, line, '\n');
	ParseUnsignedInt(line, intVal);
}

void FileIOHelper::LoadSize(std::ifstream& ifs, size_t& sizeVal)
{
	// Parse data information.
	FTDS::String line;
	GetLine(ifs, line, '\n');

	FTDS::String name;
	line.ExtractUntilLast(name, "[");

	FTDS::String typeNameStr;
	line.ExtractBracketedVal(typeNameStr, "[", "]");
	line.Clear();

	// Parse the actual data.
	GetLine(ifs, line, '\n');
	ParseSize(line, sizeVal);
}

void FileIOHelper::LoadFloat(std::ifstream& ifs, float& floatVal)
{
	// Parse data information.
	FTDS::String line;
	GetLine(ifs, line, '\n');

	FTDS::String name;
	line.ExtractUntilLast(name, "[");

	FTDS::String typeNameStr;
	line.ExtractBracketedVal(typeNameStr, "[", "]");
	line.Clear();

	// Parse the actual data.
	GetLine(ifs, line, '\n');
	ParseFloat(line, floatVal);
}

void FileIOHelper::LoadBool(std::ifstream& ifs, bool& boolVal)
{
	// Parse data information.
	FTDS::String line;
	GetLine(ifs, line, '\n');

	FTDS::String name;
	line.ExtractUntilLast(name, "[");

	FTDS::String typeNameStr;
	line.ExtractBracketedVal(typeNameStr, "[", "]");
	line.Clear();

	// Parse the actual data.
	GetLine(ifs, line, '\n');
	ParseBool(line, boolVal);
}

void FileIOHelper::LoadBasicString(std::ifstream& ifs, FTDS::String& strVal)
{
	// Parse data information.
	FTDS::String line;
	GetLine(ifs, line, '\n');

	FTDS::String name;
	line.ExtractUntilLast(name, "[");

	FTDS::String typeNameStr;
	line.ExtractBracketedVal(typeNameStr, "[", "]");
	line.Clear();

	// Parse the actual data.
	GetLine(ifs, line, '\n');
	ParseString(line, strVal);

	strVal.ExtractFromLast("\t");
}

void FileIOHelper::LoadVector2(std::ifstream& ifs, FTVector2& vec2)
{
	// Parse data information.
	FTDS::String line;
	GetLine(ifs, line, '\n');

	FTDS::String name;
	line.ExtractUntilLast(name, "[");

	FTDS::String typeNameStr;
	line.ExtractBracketedVal(typeNameStr, "[", "]");
	line.Clear();

	// Parse the actual data.
	GetLine(ifs, line, '\n');
	ParseVector2(line, vec2);
}

void FileIOHelper::LoadVector2(std::ifstream& ifs, b2Vec2& vec2)
{
	// Parse data information.
	FTDS::String line;
	GetLine(ifs, line, '\n');

	FTDS::String name;
	line.ExtractUntilLast(name, "[");

	FTDS::String typeNameStr;
	line.ExtractBracketedVal(typeNameStr, "[", "]");
	line.Clear();

	// Parse the actual data.
	GetLine(ifs, line, '\n');
	ParseVector2(line, vec2);
}

void FileIOHelper::LoadVector2(std::ifstream& ifs, DirectX::XMFLOAT2& vec2)
{
	// Parse data information.
	FTDS::String line;
	GetLine(ifs, line, '\n');

	FTDS::String name;
	line.ExtractUntilLast(name, "[");

	FTDS::String typeNameStr;
	line.ExtractBracketedVal(typeNameStr, "[", "]");
	line.Clear();

	// Parse the actual data.
	GetLine(ifs, line, '\n');
	ParseVector2(line, vec2);
}

void FileIOHelper::LoadVector3(std::ifstream& ifs, FTVector3& vec3)
{
	// Parse data information.
	FTDS::String line;
	GetLine(ifs, line, '\n');

	FTDS::String name;
	line.ExtractUntilLast(name, "[");

	FTDS::String typeNameStr;
	line.ExtractBracketedVal(typeNameStr, "[", "]");
	line.Clear();

	// Parse the actual data.
	GetLine(ifs, line, '\n');
	ParseVector3(line, vec3);
}

void FileIOHelper::LoadVector4(std::ifstream& ifs, FTVector4& vec4)
{
	// Parse data information.
	FTDS::String line;
	GetLine(ifs, line, '\n');

	FTDS::String name;
	line.ExtractUntilLast(name, "[");

	FTDS::String typeNameStr;
	line.ExtractBracketedVal(typeNameStr, "[", "]");
	line.Clear();

	// Parse the actual data.
	GetLine(ifs, line, '\n');
	ParseVector4(line, vec4);
}

void FileIOHelper::LoadVector4(std::ifstream& ifs, DirectX::XMFLOAT4& vec4)
{
	// Parse data information.
	FTDS::String line;
	GetLine(ifs, line, '\n');

	FTDS::String name;
	line.ExtractUntilLast(name, "[");

	FTDS::String typeNameStr;
	line.ExtractBracketedVal(typeNameStr, "[", "]");
	line.Clear();

	// Parse the actual data.
	GetLine(ifs, line, '\n');
	ParseVector4(line, vec4);
}

void FileIOHelper::ParseVector3(FTDS::String& line, FTVector3& arg)
{
	line.ExtractBracketedVal(line, "(", ")");

	FTDS::String xStr;
	FTDS::String yStr;
	FTDS::String zStr;

	line.ExtractUntilFirst(xStr, ",");
	line.ExtractBracketedVal(yStr, ",", ",");
	line.ExtractFromLast(zStr, ",");

	float x = std::stof(xStr.C_Str());
	float y = std::stof(yStr.C_Str());
	float z = std::stof(zStr.C_Str());

	arg = FTVector3(x, y, z);
}

void FileIOHelper::ParseVector2(FTDS::String& line, FTVector2& arg)
{
	line.ExtractBracketedVal(line, "(", ")");

	FTDS::String xStr;
	FTDS::String yStr;

	line.ExtractUntilFirst(xStr, ",");
	line.ExtractFromLast(yStr, ",");

	float x = std::stof(xStr.C_Str());
	float y = std::stof(yStr.C_Str());

	arg = FTVector2(x, y);
}

void FileIOHelper::ParseVector2(FTDS::String& line, b2Vec2& arg)
{
	line.ExtractBracketedVal(line, "(", ")");

	FTDS::String xStr;
	FTDS::String yStr;

	line.ExtractUntilFirst(xStr, ",");
	line.ExtractUntilLast(yStr, ",");

	float x = std::stof(xStr.C_Str());
	float y = std::stof(yStr.C_Str());

	arg = FTVector2(x, y).GetB2Vec2();
}

void FileIOHelper::ParseVector2(FTDS::String& line, DirectX::XMFLOAT2& arg)
{
	line.ExtractBracketedVal(line, "(", ")");

	FTDS::String xStr;
	FTDS::String yStr;

	line.ExtractUntilFirst(xStr, ",");
	line.ExtractUntilLast(yStr, ",");

	float x = std::stof(xStr.C_Str());
	float y = std::stof(yStr.C_Str());

	arg = DirectX::XMFLOAT2(x, y);
}

void FileIOHelper::ParseVector4(FTDS::String& line, FTVector4& arg)
{
	line.ExtractBracketedVal(line, "(", ")");

	// Values at both ends.
	FTDS::String xStr;
	FTDS::String wStr;

	line.ExtractUntilFirst(xStr, ",");
	line.ExtractUntilLast(wStr, ",");

	// Values inbetween others.
	FTDS::String inBetw;
	FTDS::String yStr;
	FTDS::String zStr;

	line.ExtractBracketedVal(inBetw, ",", ",");
	inBetw.ExtractUntilFirst(yStr, ",");
	inBetw.ExtractUntilLast(zStr, ",");

	float x = std::stof(xStr.C_Str());
	float y = std::stof(yStr.C_Str());
	float z = std::stof(zStr.C_Str());
	float w = std::stof(zStr.C_Str());

	arg = FTVector4(x, y, z, w);
}

void FileIOHelper::ParseVector4(FTDS::String& line, DirectX::XMFLOAT4& arg)
{
	line.ExtractBracketedVal(line, "(", ")");

	// Values at both ends.
	FTDS::String xStr;
	FTDS::String wStr;

	line.ExtractUntilFirst(xStr, ",");
	line.ExtractUntilLast(wStr, ",");

	// Values inbetween others.
	FTDS::String inBetw;
	FTDS::String yStr;
	FTDS::String zStr;

	line.ExtractBracketedVal(inBetw, ",", ",");
	inBetw.ExtractUntilFirst(yStr, ",");
	inBetw.ExtractUntilLast(zStr, ",");

	float x = std::stof(xStr.C_Str());
	float y = std::stof(yStr.C_Str());
	float z = std::stof(zStr.C_Str());
	float w = std::stof(zStr.C_Str());

	arg = DirectX::XMFLOAT4(x, y, z, w);
}

void FileIOHelper::ParseInt(FTDS::String& line, int& arg)
{
	// Remove tab identation.
	size_t tabEnd = line.RFind("\t");
	line.SubStr(tabEnd, line.GetLength());

	arg = std::stoi(line.C_Str());
}

void FileIOHelper::ParseUnsignedInt(FTDS::String& line, unsigned int& arg)
{
	// Remove tab identation.
	size_t tabEnd = line.RFind("\t");
	line.SubStr(tabEnd, line.GetLength());

	arg = std::stoul(line.C_Str(), nullptr, 0);
}

void FileIOHelper::ParseSize(FTDS::String& line, size_t& arg)
{
	// Remove tab identation.
	size_t tabEnd = line.RFind("\t");
	line.SubStr(tabEnd, line.GetLength());

	arg = static_cast<size_t>(std::stoul(line.C_Str(), nullptr, 0));
}

void FileIOHelper::ParseFloat(FTDS::String& line, float& arg)
{
	// Remove tab identation.
	size_t tabEnd = line.RFind("\t");
	line.SubStr(tabEnd, line.GetLength());

	arg = std::stof(line.C_Str());
}

void FileIOHelper::ParseBool(FTDS::String& line, bool& arg)
{
	// Remove tab identation.
	line.ExtractFromLast("\t");
	arg = StrToBool(line);
}

void FileIOHelper::ParseString(FTDS::String& line, FTDS::String& arg)
{
	// Remove tab identation.
	size_t tabEnd = line.RFind("\t");
	line.SubStr(tabEnd, line.GetLength());

	arg.Assign(line);
}

void FileIOHelper::BeginDataPackSave(std::ofstream& ofs, FTDS::String dataPackKey)
{
	mDataPackIdent = mItemCounts.size();
	mItemIdent	   = FTDS::String(mDataPackIdent + 1, '\t');

	if (0 < mItemCounts.size())
	{
		++mItemCounts.back();
	}
	mItemCounts.push_back(0);
	mCurrentDataPack.push_back(dataPackKey);

	std::cout << "Saving data pack : " << dataPackKey.C_Str() << '\n';
}

void FileIOHelper::EndDataPackSave(std::ofstream& ofs, FTDS::String dataPackKey)
{
	assert(mCurrentDataPack.back() == dataPackKey);

	int itemCount = mItemCounts.back();
	mItemCounts.pop_back();
	mCurrentDataPack.pop_back();
	mDataPackIdent = mItemCounts.size();
	mItemIdent	   = FTDS::String(mDataPackIdent + 1, '\t');

	// Returning to the datapack ident level
	FTDS::String title = FTDS::String(mDataPackIdent, '\t') + dataPackKey.C_Str() + "<" + std::to_string(itemCount).c_str() + ">";
	mDataBuffer.push_back(title);
}

void FileIOHelper::SaveBufferToFile(std::ofstream& ofs)
{
	// Key & Value pairs = size() * 2
	std::list<FTDS::String>::reverse_iterator iter = mDataBuffer.rbegin();
	for (; iter != mDataBuffer.rend(); ++iter)
	{
		ofs << (*iter).C_Str() << '\n';
	}
	mDataBuffer.clear();
}

void FileIOHelper::SaveVector3(std::ofstream& ofs, const FTDS::String& valName, const FTVector3& vec3)
{
	FTDS::String itemTitle = mItemIdent + valName + "[Vector3]" + "\n";
	FTDS::String item	   = mItemIdent + 
		"(" + std::to_string(vec3.x).c_str() + "," + std::to_string(vec3.y).c_str() + "," + std::to_string(vec3.z).c_str() + ")";
	mDataBuffer.push_back(itemTitle + item);
	++mItemCounts.back();
}

void FileIOHelper::SaveVector2(std::ofstream& ofs, const FTDS::String& valName, const FTVector2& vec2)
{
	FTDS::String itemTitle = mItemIdent + valName + "[Vector2]" + "\n";
	FTDS::String item	   = mItemIdent + "(" + std::to_string(vec2.x).c_str() + "," + std::to_string(vec2.y).c_str() + ")";
	mDataBuffer.push_back(itemTitle + item);
	++mItemCounts.back();
}

void FileIOHelper::SaveVector2(std::ofstream& ofs, const FTDS::String& valName, const DirectX::XMFLOAT2& vec2)
{
	FTDS::String itemTitle = mItemIdent + valName + "[Vector2]" + "\n";
	FTDS::String item	   = mItemIdent + "(" + std::to_string(vec2.x).c_str() + "," + std::to_string(vec2.y).c_str() + ")";
	mDataBuffer.push_back(itemTitle + item);
	++mItemCounts.back();
}

#ifdef FOXTROT_EDITOR
void FileIOHelper::SaveVector2(std::ofstream& ofs, const FTDS::String& valName, const b2Vec2& vec2)
{
	FTDS::String itemTitle = mItemIdent + valName + "[Vector2]" + "\n";
	FTDS::String item	   = mItemIdent + "(" + std::to_string(vec2.x).c_str() + "," + std::to_string(vec2.y).c_str() + ")";
	mDataBuffer.push_back(itemTitle + item);
	++mItemCounts.back();
}
#endif // FOXTROT_EDITOR

void FileIOHelper::SaveVector4(std::ofstream& ofs, const FTDS::String& valName, const FTVector4& vec4)
{
	FTDS::String itemTitle = mItemIdent + valName + "[Vector4]" + "\n";
	FTDS::String item =
		mItemIdent +
		"(" +
		std::to_string(vec4.x).c_str() + "," + std::to_string(vec4.y).c_str() +
		std::to_string(vec4.z).c_str() + "," + std::to_string(vec4.a).c_str() +
		")";
	mDataBuffer.push_back(itemTitle + item);
	++mItemCounts.back();
}

void FileIOHelper::SaveVector4(std::ofstream& ofs, const FTDS::String& valName, const DirectX::XMFLOAT4& vec4)
{
	FTDS::String itemTitle = mItemIdent + valName + "[Vector4]" + "\n";
	FTDS::String item =
		mItemIdent +
		"(" +
		std::to_string(vec4.x).c_str() + "," + std::to_string(vec4.y).c_str() + 
		std::to_string(vec4.z).c_str() + "," + std::to_string(vec4.w).c_str() +
		")";
	mDataBuffer.push_back(itemTitle + item);
	++mItemCounts.back();
}

void FileIOHelper::SaveInt(std::ofstream& ofs, const FTDS::String& valName, const int& intVal)
{
	FTDS::String itemTitle = mItemIdent + valName + "[int]" + "\n";
	FTDS::String item	   = mItemIdent + std::to_string(intVal).c_str();
	mDataBuffer.push_back(itemTitle + item);
	++mItemCounts.back();
}

void FileIOHelper::SaveUnsignedInt(std::ofstream& ofs, const FTDS::String& valName, const unsigned int& intVal)
{
	FTDS::String itemTitle = mItemIdent + valName + "[unsigned int]" + "\n";
	FTDS::String item	   = mItemIdent + std::to_string(intVal).c_str();
	mDataBuffer.push_back(itemTitle + item);
	++mItemCounts.back();
}

void FileIOHelper::SaveSize(std::ofstream& ofs, const FTDS::String& valName, const size_t& sizeVal)
{
	FTDS::String itemTitle = mItemIdent + valName + "[size]" + "\n";
	FTDS::String item	   = mItemIdent + std::to_string(sizeVal).c_str();
	mDataBuffer.push_back(itemTitle + item);
	++mItemCounts.back();
}

void FileIOHelper::SaveFloat(std::ofstream& ofs, const FTDS::String& valName, const float& floatVal)
{
	FTDS::String itemTitle = mItemIdent + valName + "[float]" + "\n";
	FTDS::String item	   = mItemIdent + std::to_string(floatVal).c_str();
	mDataBuffer.push_back(itemTitle + item);
	++mItemCounts.back();
}

void FileIOHelper::SaveString(std::ofstream& ofs, FTDS::String& valName, const FTDS::String& strVal)
{
	FTDS::String itemTitle = mItemIdent + valName + "[string]" + "\n";
	FTDS::String item	   = mItemIdent + strVal;
	mDataBuffer.push_back(itemTitle + item);
	++mItemCounts.back();
}

void FileIOHelper::SaveString(std::ofstream& ofs, const char* valName, const FTDS::String& strVal)
{
	FTDS::String itemTitle = mItemIdent + valName + "[string]" + "\n";
	FTDS::String item	   = mItemIdent + strVal.C_Str();
	mDataBuffer.push_back(itemTitle + item);
	++mItemCounts.back();
}

void FileIOHelper::SaveString(std::ofstream& ofs, const char* valName, const char* strVal)
{
	FTDS::String itemTitle = mItemIdent + valName + "[string]" + "\n";
	FTDS::String item = mItemIdent + strVal;
	mDataBuffer.push_back(itemTitle + item);
	++mItemCounts.back();
}

void FileIOHelper::SaveBool(std::ofstream& ofs, const FTDS::String& valName, const bool& boolVal)
{
	FTDS::String itemTitle = mItemIdent + valName + "[bool]" + "\n";
	FTDS::String item	   = mItemIdent + ToString(boolVal);
	mDataBuffer.push_back(itemTitle + item);
	++mItemCounts.back();
}

bool FileIOHelper::GetLine(std::ifstream& ifs, FTDS::String& str, char delimiter)
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