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

std::list<std::string> FileIOHelper::mDataBuffer = {};
std::list<int> FileIOHelper::mItemCounts = {};
std::list<std::string> FileIOHelper::mCurrentDataPack = {};
int FileIOHelper::mDataPackIdent = 0;
std::string FileIOHelper::mItemIdent = std::string(mDataPackIdent, '\t');

// This function is less safe since there is no assert to check data pack name
std::pair<size_t, std::string> FileIOHelper::BeginDataPackLoad(std::ifstream& ifs)
{
    std::string loadedDataPackKey;
    std::getline(ifs, loadedDataPackKey, '\n');
    LTrim(loadedDataPackKey);

    // Parse data pack key 
    std::string name = ExtractUntil(loadedDataPackKey, '<');
    size_t count = std::stoi(GetBracketedVal(loadedDataPackKey, '<', '>'));

    //std::cout << "Parsing data pack : " << name << '\n';
    //std::cout << name << '\n';

    std::pair<size_t, std::string> result{ count, name };
    return result;
}

std::pair<size_t, std::string> FileIOHelper::BeginDataPackLoad(std::ifstream& ifs, std::string dataPackKey)
{
    std::string loadedDataPackKey;
    std::getline(ifs, loadedDataPackKey, '\n');
    LTrim(loadedDataPackKey);

    // Parse data pack key 
    std::string name = ExtractUntil(loadedDataPackKey, '<');
    size_t count = std::stoi(GetBracketedVal(loadedDataPackKey, '<', '>'));

    assert(name == dataPackKey);

    //std::cout << "Parsing data pack : " << name << '\n';
    //std::cout << name << '\n';

    std::pair<size_t, std::string> result{ count, name };
    return result;
}

void FileIOHelper::LoadInt(std::ifstream& ifs, int& intVal)
{
    // Parse data information.
    std::string line;
    std::getline(ifs, line, '\n');

    std::string name = ExtractUntil(line, '[');
    std::string typeNameStr = GetBracketedVal(line, '[', ']');
    line.clear();

    // Parse the actual data.
    std::getline(ifs, line, '\n');
    ParseInt(line, intVal);
}

void FileIOHelper::LoadUnsignedInt(std::ifstream& ifs, unsigned int& intVal)
{
    // Parse data information.
    std::string line;
    std::getline(ifs, line, '\n');

    std::string name = ExtractUntil(line, '[');
    std::string typeNameStr = GetBracketedVal(line, '[', ']');
    line.clear();

    // Parse the actual data.
    std::getline(ifs, line, '\n');
    ParseUnsignedInt(line, intVal);
    LogInt(intVal);
}

void FileIOHelper::LoadSize(std::ifstream& ifs, size_t& sizeVal)
{
    // Parse data information.
    std::string line;
    std::getline(ifs, line, '\n');

    std::string name = ExtractUntil(line, '[');
    std::string typeNameStr = GetBracketedVal(line, '[', ']');
    line.clear();

    // Parse the actual data.
    std::getline(ifs, line, '\n');
    ParseSize(line, sizeVal);
    LogInt(sizeVal);
}

void FileIOHelper::LoadFloat(std::ifstream& ifs, float& floatVal)
{
    // Parse data information.
    std::string line;
    std::getline(ifs, line, '\n');

    std::string name = ExtractUntil(line, '[');
    std::string typeNameStr = GetBracketedVal(line, '[', ']');
    line.clear();

    // Parse the actual data.
    std::getline(ifs, line, '\n');
    ParseFloat(line, floatVal);
}

void FileIOHelper::LoadBool(std::ifstream& ifs, bool& boolVal)
{
    // Parse data information.
    std::string line;
    std::getline(ifs, line, '\n');

    std::string name = ExtractUntil(line, '[');
    std::string typeNameStr = GetBracketedVal(line, '[', ']');
    line.clear();

    // Parse the actual data.
    std::getline(ifs, line, '\n');
    ParseBool(line, boolVal);
}

void FileIOHelper::LoadBasicString(std::ifstream& ifs, std::string& strVal)
{
    // Parse data information.
    std::string line;
    std::getline(ifs, line, '\n');

    std::string name = ExtractUntil(line, '[');
    std::string typeNameStr = GetBracketedVal(line, '[', ']');
    line.clear();

    // Parse the actual data.
    std::getline(ifs, line, '\n');
    ParseString(line, strVal);
}

void FileIOHelper::LoadVector2(std::ifstream& ifs, FTVector2& vec2)
{
    // Parse data information.
    std::string line;
    std::getline(ifs, line, '\n');

    std::string name = ExtractUntil(line, '[');
    std::string typeNameStr = GetBracketedVal(line, '[', ']');
    line.clear();

    // Parse the actual data.
    std::getline(ifs, line, '\n');
    ParseVector2(line, vec2);
}

void FileIOHelper::LoadVector2(std::ifstream& ifs, b2Vec2& vec2)
{
    // Parse data information.
    std::string line;
    std::getline(ifs, line, '\n');

    std::string name = ExtractUntil(line, '[');
    std::string typeNameStr = GetBracketedVal(line, '[', ']');
    line.clear();

    // Parse the actual data.
    std::getline(ifs, line, '\n');
    ParseVector2(line, vec2);
}

void FileIOHelper::LoadVector3(std::ifstream& ifs, FTVector3& vec3)
{
    // Parse data information.
    std::string line;
    std::getline(ifs, line, '\n');

    std::string name = ExtractUntil(line, '[');
    std::string typeNameStr = GetBracketedVal(line, '[', ']');
    line.clear();

    // Parse the actual data.
    std::getline(ifs, line, '\n');
    ParseVector3(line, vec3);
}

void FileIOHelper::ParseVector3(std::string& line, FTVector3& arg)
{
    line = GetBracketedVal(line, '(', ')');
    float x = std::stof(ExtractUntil(line, ','));
    float y = std::stof(ExtractUntil(line, ','));
    float z = std::stof(ExtractUntil(line, ')'));

    arg = FTVector3(x, y, z);
}

void FileIOHelper::ParseVector2(std::string& line, FTVector2& arg)
{
    line = GetBracketedVal(line, '(', ')');
    float x = std::stof(ExtractUntil(line, ','));
    float y = std::stof(ExtractUntil(line, ','));

    arg = FTVector2(x, y);
}

void FileIOHelper::ParseVector2(std::string& line, b2Vec2& arg)
{
    line = GetBracketedVal(line, '(', ')');
    float x = std::stof(ExtractUntil(line, ','));
    float y = std::stof(ExtractUntil(line, ','));

    arg = FTVector2(x, y).GetB2Vec2();
}

void FileIOHelper::ParseInt(std::string& line, int& arg)
{
    LTrim(line);
    arg = std::stoi(line);
}

void FileIOHelper::ParseUnsignedInt(std::string& line, unsigned int& arg)
{
    LTrim(line);
    arg = std::stoul(line, nullptr, 0);
}

void FileIOHelper::ParseSize(std::string& line, size_t& arg)
{
    LTrim(line);
    arg = static_cast<size_t>(std::stoul(line, nullptr, 0));
}

void FileIOHelper::ParseFloat(std::string& line, float& arg)
{
    LTrim(line);
    arg = std::stof(line);
}

void FileIOHelper::ParseBool(std::string& line, bool& arg)
{
    LTrim(line);
    arg = StrToBool(line);
}

void FileIOHelper::ParseString(std::string& line, std::string& arg)
{
    LTrim(line);
    arg.assign(line);
}

void FileIOHelper::BeginDataPackSave(std::ofstream& ofs, std::string dataPackKey)
{
    mDataPackIdent = mItemCounts.size();
    mItemIdent = std::string(mDataPackIdent + 1, '\t');

    if (0 < mItemCounts.size())
    {
        ++mItemCounts.back();
    }
    mItemCounts.push_back(0);
    mCurrentDataPack.push_back(dataPackKey);

    std::cout << "Saving data pack : " << dataPackKey << '\n';
}

void FileIOHelper::EndDataPackSave(std::ofstream& ofs, std::string dataPackKey)
{
    assert(mCurrentDataPack.back() == dataPackKey);

    int itemCount = mItemCounts.back();
    mItemCounts.pop_back();
    mCurrentDataPack.pop_back();
    mDataPackIdent = mItemCounts.size();
    mItemIdent = std::string(mDataPackIdent + 1, '\t');

    // Returning to the datapack ident level
    std::string title = std::string(mDataPackIdent, '\t') + dataPackKey + "<" + std::to_string(itemCount) + ">";
    mDataBuffer.push_back(title);
}

void FileIOHelper::SaveBufferToFile(std::ofstream& ofs)
{
    // Key & Value pairs = size() * 2
    std::list<std::string>::reverse_iterator iter = mDataBuffer.rbegin();
    for (; iter != mDataBuffer.rend(); ++iter) {
        ofs << *iter << '\n';
    }
    mDataBuffer.clear();
}

void FileIOHelper::SaveVector3(std::ofstream& ofs, const std::string valName, const FTVector3& vec3)
{
    std::string itemTitle = mItemIdent + valName + "[Vector3]" + '\n';
    std::string item = mItemIdent + "(" + std::to_string(vec3.x) + "," + std::to_string(vec3.y) + "," + std::to_string(vec3.z) + ")";
    mDataBuffer.push_back(itemTitle + item);
    ++mItemCounts.back();
}

void FileIOHelper::SaveVector2(std::ofstream& ofs, const std::string valName, const FTVector2& vec2)
{
    std::string itemTitle = mItemIdent + valName + "[Vector2]" + '\n';
    std::string item = mItemIdent + "(" + std::to_string(vec2.x) + "," + std::to_string(vec2.y) + ")";
    mDataBuffer.push_back(itemTitle + item);
    ++mItemCounts.back();
}

#ifdef FOXTROT_EDITOR
void FileIOHelper::SaveVector2(std::ofstream& ofs, const std::string valName, const b2Vec2& vec2)
{
    std::string itemTitle = mItemIdent + valName + "[Vector2]" + '\n';
    std::string item = mItemIdent + "(" + std::to_string(vec2.x) + "," + std::to_string(vec2.y) + ")";
    mDataBuffer.push_back(itemTitle + item);
    ++mItemCounts.back();
}
#endif // FOXTROT_EDITOR


void FileIOHelper::SaveInt(std::ofstream& ofs, const std::string valName, const int& intVal)
{
    std::string itemTitle = mItemIdent + valName + "[int]" + '\n';
    std::string item = mItemIdent + std::to_string(intVal);
    mDataBuffer.push_back(itemTitle + item);
    ++mItemCounts.back();
}

void FileIOHelper::SaveUnsignedInt(std::ofstream& ofs, const std::string valName, const unsigned int& intVal)
{
    std::string itemTitle = mItemIdent + valName + "[unsigned int]" + '\n';
    std::string item = mItemIdent + std::to_string(intVal);
    mDataBuffer.push_back(itemTitle + item);
    ++mItemCounts.back();
}

void FileIOHelper::SaveFloat(std::ofstream& ofs, const std::string valName, const float& floatVal)
{
    std::string itemTitle = mItemIdent + valName + "[float]" + '\n';
    std::string item = mItemIdent + std::to_string(floatVal);
    mDataBuffer.push_back(itemTitle + item);
    ++mItemCounts.back();
}

void FileIOHelper::SaveString(std::ofstream& ofs, const std::string valName, const std::string& strVal)
{
    std::string itemTitle = mItemIdent + valName + "[string]" + '\n';
    std::string item = mItemIdent + strVal;
    mDataBuffer.push_back(itemTitle + item);
    ++mItemCounts.back();
}

void FileIOHelper::SaveBool(std::ofstream& ofs, const std::string valName, const bool& boolVal)
{
    std::string itemTitle = mItemIdent + valName + "[bool]" + '\n';
    std::string item = mItemIdent + ToString(boolVal);
    mDataBuffer.push_back(itemTitle + item);
    ++mItemCounts.back();
}

std::string FileIOHelper::GetBracketedVal(std::string& str, const char left, const char right) {
    size_t begin = str.find(left);
    size_t end = str.find(right);
    return str.substr(begin + 1, end - (begin + 1));
}