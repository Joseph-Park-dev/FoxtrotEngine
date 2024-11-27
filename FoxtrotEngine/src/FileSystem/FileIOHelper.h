#pragma once
#include <filesystem>
#include <list>

class FTVector2;
class FTVector3;
struct b2Vec2;

class FileIOHelper
{
public:
	static std::pair<size_t, std::string> BeginDataPackLoad(std::ifstream& ifs);
	static std::pair<size_t, std::string> BeginDataPackLoad(std::ifstream& ifs, std::string dataPackKey);

	static void   LoadInt(std::ifstream& ifs, int& intVal);
	static void   LoadUnsignedInt(std::ifstream& ifs, unsigned int& intVal);
	static void   LoadSize(std::ifstream& ifs, size_t& sizeVal);
	static void   LoadFloat(std::ifstream& ifs, float& floatVal);
	static void	  LoadBool(std::ifstream& ifs, bool& boolVal);
	static void   LoadBasicString(std::ifstream& ifs, std::string& strVal);
	static void   LoadVector2(std::ifstream& ifs, FTVector2& vec2);
	static void	  LoadVector2(std::ifstream& ifs, b2Vec2& vec2);
	static void   LoadVector3(std::ifstream& ifs, FTVector3& vec3);

	static void	  ParseVector3(std::string& line, FTVector3& arg);
	static void	  ParseVector2(std::string& line, FTVector2& arg);
	static void	  ParseVector2(std::string& line, b2Vec2& arg);

	static void	  ParseInt(std::string& line, int& arg);
	static void   ParseUnsignedInt(std::string& line, unsigned int& arg);
	static void   ParseSize(std::string& line, size_t& arg);
	static void	  ParseFloat(std::string& line, float& arg);
	static void	  ParseBool(std::string& line, bool& arg);
	static void   ParseString(std::string& line, std::string& arg);

public:
	// Initialize identation, start DBG_NEW list of item
	static void	BeginDataPackSave(std::ofstream& ofs, std::string dataPackKey);
	static void	EndDataPackSave(std::ofstream& ofs, std::string dataPackKey);
	static void SaveBufferToFile(std::ofstream& ofs);

	static void SaveVector3(std::ofstream& ofs, const std::string valName, const FTVector3& vec3);
	static void SaveVector2(std::ofstream& ofs, const std::string valName, const FTVector2& vec2);
	static void SaveVector2(std::ofstream& ofs, const std::string valName, const b2Vec2& vec2);

	static void SaveInt(std::ofstream& ofs, const std::string valName, const int& intVal);
	static void SaveUnsignedInt(std::ofstream& ofs, const std::string valName, const unsigned int& intVal);
	static void SaveFloat(std::ofstream& ofs, const std::string valName, const float& floatVal);
	static void SaveString(std::ofstream& ofs, const std::string valName, const std::string& strVal);
	static void SaveBool(std::ofstream& ofs, const std::string valName, const bool& boolVal);

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
	static std::string GetBracketedVal(std::string& str, const char left, const char right);
};