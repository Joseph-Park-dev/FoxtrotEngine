// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Defines the global utility functions.
/// </summary>

#pragma once
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <iterator>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <filesystem>

#include "Static/FTString.h"
#include "Math/FTMath.h"
#include "Debugging/DebugMemAlloc.h"

// static FTDS::String ToString(const wchar_t* text)
//{
//	size_t i = 0;
//	size_t size = std::wcslen(text);
//	char* str = DBG_NEW char[size];
//	if (size > 0) {
//		wcstombs_s(&i, str, size, text, _TRUNCATE);
//	}
//	return str;
// }

namespace Core
{
	inline void LogInt(int val)
	{
		printf("int : %d\n", val);
	}

	inline void LogInt(FTDS::String message, int val)
	{
		FTDS::String msg = message + std::to_string(val).c_str() + "\n";
		printf(msg.C_Str());
	}

	inline void LogFloat(float val)
	{
		printf("float : %f\n", val);
	}

	inline void LogFloat(FTDS::String message, float val)
	{
		FTDS::String msg = message + std::to_string(val).c_str() + "\n";
		printf(msg.C_Str());
	}

	inline void LogVector2(Math::FTVector2 val)
	{
		printf("X : %f Y : %f\n", val.x, val.y);
	}

	inline void LogVector2(FTDS::String message, Math::FTVector2 val)
	{
		printf("%s, X : %f Y : %f\n", message.C_Str(), val.x, val.y);
	}

	inline void LogVector3(FTDS::String message, Math::FTVector3 val)
	{
		printf("%s, X : %f Y : %f Z : %f\n", message.C_Str(), val.x, val.y, val.z);
	}

	inline void LogBool(bool val)
	{
		if (val)
			printf("Bool Log : True\n");
		else
			printf("Bool Log : False\n");
	}

	inline void LogString(FTDS::String val)
	{
		printf("%s\n", val.C_Str());
	}

	inline void LogString(const char* val)
	{
		printf("%s\n", val);
	}

	inline void LogString(std::wstring message, std::wstring val)
	{
		printf("%ls\n", (message + L" :" + val).c_str());
	}

	inline void LogString(FTDS::String message, FTDS::String val)
	{
		message.Append(" :");
		message.Append(val);
		printf("%s\n", message.C_Str());
	}

	// static std::wstring ToWString(const FTDS::String& text)
	//{
	//	std::wstring wStr;
	//	wStr.assign(text.begin(), text.end());
	//	return wStr;
	// }
	//
	// static FTDS::String ToString(const std::wstring& text)
	//{
	//	FTDS::String str;
	//	str.assign(text.begin(), text.end());
	//	return str;
	// }

	static char* ToString(const wchar_t* text)
	{
		size_t i	= 0;
		size_t size = std::wcslen(text);
		char*  str	= DBG_NEW char[size];
		if (0 < size)
		{
			wcstombs_s(&i, str, size, text, _TRUNCATE);
			return str;
		}
		return nullptr;
	}

	static const char* ToString(bool boolVal)
	{
		return boolVal ? "true" : "false";
	}

	static const bool StrToBool(FTDS::String& str)
	{
		return str.Equal("true") ? true : false;
	}

	inline FTDS::String ExtractFileName(const char* path)
	{
		std::filesystem::path p(path);
		FTDS::String		  pathStr = FTDS::String(p.string().c_str());
		int					  idx	  = pathStr.RFind("\\");
		pathStr.SubStr(idx + 1, pathStr.GetLength() - idx); // 1 -> "\\" should be omitted.
		return pathStr;
	}

	inline FTDS::String ExtractFileName(FTDS::String* path, FTDS::String& out)
	{
		std::filesystem::path p(path->C_Str());
		out.Assign(*path);
		int idx = out.RFind("\\");
		out.SubStr(idx + 1, out.GetLength() - idx); // 1 -> "\\" should be omitted.
	}

	inline void ExtractFileName(const FTDS::String& path, FTDS::String& out)
	{
		std::filesystem::path p(path.C_Str());
		out.Assign(path);
		int idx = out.RFind("\\");
		out.SubStr(idx + 1, out.GetLength() - idx); // 1 -> "\\" should be omitted.
	}

	inline FTDS::String ExtractFileType(const char* path)
	{
		std::filesystem::path p(path);
		FTDS::String		  pathStr = FTDS::String(p.string().c_str());
		int					  idx	  = pathStr.RFind(".");
		pathStr.SubStr(idx, pathStr.GetLength() - idx);
		return pathStr;
	}

	inline void ExtractUntil(FTDS::String& line, const char* end)
	{
		int typeBeg = line.LFind(end);
		line.SubStr(0, typeBeg);
	}

	inline void RemoveSuffix(FTDS::String& line, const char* start)
	{
		size_t typeBegin = line.RFind(start);
		line.SubStr(0, typeBegin);
	}

	inline bool EndsWith(
		const std::wstring& fullString,
		const std::wstring& ending)
	{
		// Check if the ending string is longer than the full
		// string
		if (ending.size() > fullString.size())
			return false;

		// Compare the ending of the full string with the target
		// ending
		return fullString.compare(fullString.size() - ending.size(), ending.size(), ending) == 0;
	}

	inline void ReplaceSuffix(FTDS::String& curr, FTDS::String prevSuffix, FTDS::String postSuffix)
	{
		curr.SubStr(0, curr.GetLength() - prevSuffix.GetLength());
		curr.Append(postSuffix);
	}

	template <typename T>
	size_t GetArrayLength(T arr)
	{
		return sizeof(arr) / sizeof(T);
	}

	template <typename T>
	void SafeDeleteArray(T** arr)
	{
		size_t length = GetArrayLength(arr);
		for (size_t i = 0; i < length; ++i)
		{
			delete arr[i];
		}
		delete[] arr;
	}

	template <typename T>
	void Safe_Delete_Vector(std::vector<T>& vec)
	{
		for (int i = 0; i < vec.size(); ++i)
		{
			if (vec[i] != nullptr)
			{
				delete vec[i];
			}
		}
		vec.clear();
	}

	template <class TKey, class TVal>
	void Safe_Delete_Map(std::unordered_map<TKey, TVal>& map)
	{
		for (auto i = map.begin(); i != map.end(); i++)
		{
			delete i->second;
		}
		map.clear();
	}
} // namespace Core