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

#include "FTDS/Static/FTString.h"

// static Common::FTDS::String ToString(const wchar_t* text)
//{
//	size_t i = 0;
//	size_t size = std::wcslen(text);
//	char* str = DBG_NEW char[size];
//	if (size > 0) {
//		wcstombs_s(&i, str, size, text, _TRUNCATE);
//	}
//	return str;
// }

namespace Common
{
	/// @brief Writes a formatted int diagnostic to standard output.
	/// @param val Value to assign, insert, or process.
	inline void LogInt(int val)
	{
		printf("int : %d\n", val);
	}

	/// @brief Writes a formatted int diagnostic to standard output.
	/// @param message Diagnostic text or message identifier.
	/// @param val Value to assign, insert, or process.
	inline void LogInt(Common::FTDS::String message, int val)
	{
		Common::FTDS::String msg = message + std::to_string(val).c_str() + "\n";
		printf(msg.C_Str());
	}

	/// @brief Writes a formatted float diagnostic to standard output.
	/// @param val Value to assign, insert, or process.
	inline void LogFloat(float val)
	{
		printf("float : %f\n", val);
	}

	/// @brief Writes a formatted float diagnostic to standard output.
	/// @param message Diagnostic text or message identifier.
	/// @param val Value to assign, insert, or process.
	inline void LogFloat(Common::FTDS::String message, float val)
	{
		Common::FTDS::String msg = message + std::to_string(val).c_str() + "\n";
		printf(msg.C_Str());
	}

	//inline void LogVector2(Math::FTVector2 val)
	//{
	//	printf("X : %f Y : %f\n", val.x, val.y);
	//}

	//inline void LogVector2(Common::FTDS::String message, Math::FTVector2 val)
	//{
	//	printf("%s, X : %f Y : %f\n", message.C_Str(), val.x, val.y);
	//}

	//inline void LogVector3(Common::FTDS::String message, Math::FTVector3 val)
	//{
	//	printf("%s, X : %f Y : %f Z : %f\n", message.C_Str(), val.x, val.y, val.z);
	//}

	/// @brief Writes a formatted bool diagnostic to standard output.
	/// @param val Value to assign, insert, or process.
	inline void LogBool(bool val)
	{
		if (val)
			printf("Bool Log : True\n");
		else
			printf("Bool Log : False\n");
	}

	/// @brief Writes a formatted string diagnostic to standard output.
	/// @param val Value to assign, insert, or process.
	inline void LogString(Common::FTDS::String val)
	{
		printf("%s\n", val.C_Str());
	}

	/// @brief Writes a formatted string diagnostic to standard output.
	/// @param val Value to assign, insert, or process.
	inline void LogString(const char* val)
	{
		printf("%s\n", val);
	}

	/// @brief Writes a formatted string diagnostic to standard output.
	/// @param message Diagnostic text or message identifier.
	/// @param val Value to assign, insert, or process.
	inline void LogString(std::wstring message, std::wstring val)
	{
		printf("%ls\n", (message + L" :" + val).c_str());
	}

	/// @brief Writes a formatted string diagnostic to standard output.
	/// @param message Diagnostic text or message identifier.
	/// @param val Value to assign, insert, or process.
	inline void LogString(Common::FTDS::String message, Common::FTDS::String val)
	{
		message.Append(" :");
		message.Append(val);
		printf("%s\n", message.C_Str());
	}

	// static std::wstring ToWString(const Common::FTDS::String& text)
	//{
	//	std::wstring wStr;
	//	wStr.assign(text.begin(), text.end());
	//	return wStr;
	// }
	//
	// static Common::FTDS::String ToString(const std::wstring& text)
	//{
	//	Common::FTDS::String str;
	//	str.assign(text.begin(), text.end());
	//	return str;
	// }

	/// @brief Converts the supplied value to the text representation used by the engine.
	/// @param text Null-terminated text to display, log, or convert.
	/// @return Converts the supplied value to the text representation used by the engine.
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

	/// @brief Converts the supplied value to the text representation used by the engine.
	/// @param boolVal Boolean value to serialize or convert.
	/// @return Converts the supplied value to the text representation used by the engine.
	static const char* ToString(bool boolVal)
	{
		return boolVal ? "true" : "false";
	}

	/// @brief Interprets the exact string "true" as true and all other text as false.
	/// @param str Text used by the operation.
	/// @return True only when str equals "true".
	static const bool StrToBool(Common::FTDS::String& str)
	{
		return str.Equal("true") ? true : false;
	}

	/// @brief Extracts the final filename component from a Windows-style path.
	/// @param path Filesystem path of the resource or project.
	/// @return Extracts the final filename component from a Windows-style path.
	inline Common::FTDS::String ExtractFileName(const char* path)
	{
		std::filesystem::path p(path);
		Common::FTDS::String  pathStr = Common::FTDS::String(p.string().c_str());
		int					  idx	  = pathStr.RFind("\\");
		pathStr.SubStr(idx + 1, pathStr.GetLength() - idx); // 1 -> "\\" should be omitted.
		return pathStr;
	}

	/// @brief Extracts the final filename component from a Windows-style path.
	/// @param path Filesystem path of the resource or project.
	/// @param out Receives the operation's output.
	inline void ExtractFileName(Common::FTDS::String* path, Common::FTDS::String& out)
	{
		std::filesystem::path p(path->C_Str());
		out.Assign(*path);
		int idx = out.RFind("\\");
		out.SubStr(idx + 1, out.GetLength() - idx); // 1 -> "\\" should be omitted.
	}

	/// @brief Extracts the final filename component from a Windows-style path.
	/// @param path Filesystem path of the resource or project.
	/// @param out Receives the operation's output.
	inline void ExtractFileName(const Common::FTDS::String& path, Common::FTDS::String& out)
	{
		std::filesystem::path p(path.C_Str());
		out.Assign(path);
		int idx = out.RFind("\\");
		out.SubStr(idx + 1, out.GetLength() - idx); // 1 -> "\\" should be omitted.
	}

	/// @brief Extracts the suffix beginning at the final period in a path.
	/// @param path Filesystem path of the resource or project.
	/// @return Extracts the suffix beginning at the final period in a path.
	inline Common::FTDS::String ExtractFileType(const char* path)
	{
		std::filesystem::path p(path);
		Common::FTDS::String  pathStr = Common::FTDS::String(p.string().c_str());
		int					  idx	  = pathStr.RFind(".");
		pathStr.SubStr(idx, pathStr.GetLength() - idx);
		return pathStr;
	}

	/// @brief Keeps the prefix preceding the first matching delimiter.
	/// @param line Text line to parse or modify.
	/// @param end Delimiter marking the end of the retained text.
	inline void ExtractUntil(Common::FTDS::String& line, const char* end)
	{
		int typeBeg = line.LFind(end);
		line.SubStr(0, typeBeg);
	}

	/// @brief Truncates the text at the final occurrence of the suffix marker.
	/// @param line Text line to parse or modify.
	/// @param start First character index or delimiter marking truncation.
	inline void RemoveSuffix(Common::FTDS::String& line, const char* start)
	{
		size_t typeBegin = line.RFind(start);
		line.SubStr(0, typeBegin);
	}

	/// @brief Compares the end of a wide string with the requested suffix.
	/// @param fullString Complete text whose suffix is tested.
	/// @param ending Suffix to compare against the end of the string.
	/// @return True when the compared values match; otherwise false.
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

	/// @brief Removes the previous suffix length and appends replacement text.
	/// @param curr Current string or value to modify.
	/// @param prevSuffix Suffix whose length is removed.
	/// @param postSuffix Replacement suffix text.
	inline void ReplaceSuffix(Common::FTDS::String& curr, Common::FTDS::String prevSuffix, Common::FTDS::String postSuffix)
	{
		curr.SubStr(0, curr.GetLength() - prevSuffix.GetLength());
		curr.Append(postSuffix);
	}

	/// @brief Returns the array length used by this service.
	/// @param arr Array storage accessed by the helper.
	/// @return Current array length.
	template <typename T>
	size_t GetArrayLength(T arr)
	{
		return sizeof(arr) / sizeof(T);
	}

	/// @brief Deletes pointer elements selected by the array helper, then releases the array.
	/// @param arr Array storage accessed by the helper.
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

	/// @brief Deletes the stored pointer values and clears the vector.
	/// @param vec Vector or vector container processed by the helper.
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

	/// @brief Deletes owned pointer entries and clears or releases the map.
	/// @param map Map whose pointer entries are processed.
	template <class TKey, class TVal>
	void Safe_Delete_Map(std::unordered_map<TKey, TVal>& map)
	{
		for (auto i = map.begin(); i != map.end(); i++)
		{
			delete i->second;
		}
		map.clear();
	}
} // namespace Common
