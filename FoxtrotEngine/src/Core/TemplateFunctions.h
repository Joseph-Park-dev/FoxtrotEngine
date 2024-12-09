#pragma once
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <iterator>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <directxtk/SimpleMath.h>

#ifdef FOXTROT_EDITOR
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#endif // FOXTROT_EDITOR

#include "Math/FTMath.h"
#include "Physics/Bounds.h"
#include "Debugging/DebugMemAlloc.h"

//static std::string ToString(const wchar_t* text)
//{
//	size_t i = 0;
//	size_t size = std::wcslen(text);
//	char* str = DBG_NEW char[size];
//	if (size > 0) {
//		wcstombs_s(&i, str, size, text, _TRUNCATE);
//	}
//	return str;
//}

#ifdef FOXTROT_EDITOR
inline void LogVector2(ImVec2 val)
{
	printf("X : %f Y : %f\n", val.x, val.y);
}
#endif // FOXTROT_EDITOR

inline void LogInt(int val)
{
	printf("int : %d\n", val);
}

inline void LogInt(std::string message, int val)
{
	std::string msg = message + std::to_string(val) + "\n";
	printf(msg.c_str());
}

inline void LogFloat(float val)
{
	printf("float : %f\n", val);
}

inline void LogFloat(std::string message, float val)
{
	std::string msg = message + std::to_string(val) + "\n";
	printf(msg.c_str());
}

inline void LogVector2(FTVector2 val)
{
	printf("X : %f Y : %f\n", val.x, val.y);
}

inline void LogVector3(DirectX::SimpleMath::Vector3 val)
{
	printf("X : %f Y : %f Z : %f\n", val.x, val.y, val.z);
}

inline void LogVector2(std::string message, FTVector2 val)
{
	printf("%s, X : %f Y : %f\n", message.c_str(), val.x, val.y);
}

inline void LogVector3(std::string message, FTVector3 val)
{
	printf("%s, X : %f Y : %f Z : %f\n", message.c_str(), val.x, val.y, val.z);
}

inline void LogRect(Bounds bound)
{
	printf("Pos X : %f, Pos Y : %f, Width : %f, Height : %f\n", bound.x, bound.y, bound.w, bound.h);
}

inline void LogBool(bool val)
{
	if (val)
		printf("Bool Log : True\n");
	else
		printf("Bool Log : False\n");
}

inline void LogString(std::wstring val)
{
	printf("%ls\n", val.c_str());
}

inline void LogString(std::string val)
{
	printf("%s\n", val.c_str());
}

inline void LogString(std::wstring message, std::wstring val)
{
	printf("%ls\n", (message + L" :" + val).c_str());
}

inline void LogString(std::string message, std::string val)
{
	printf("%s\n", (message + " :" + val).c_str());
}

static std::wstring ToWString(const std::string& text)
{   
	std::wstring wStr;    
	wStr.assign(text.begin(), text.end());
	return wStr;
}

static std::string ToString(const std::wstring& text)
{
	std::string str;
	str.assign(text.begin(), text.end());
	return str;
}

static char* ToString(const wchar_t* text)
{
	size_t i = 0;
	size_t size = std::wcslen(text);
	char* str = DBG_NEW char[size];
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

static const bool StrToBool(std::string& str) 
{
	return str == "true" ? true : false;
}

inline void LTrim(std::string& str) {
	str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](unsigned char ch) {
		return !(ch == '\t');
		}));
}

inline std::string ExtractUntil(std::string& line, const char end) {
	size_t typeBeg = line.find(end);
	std::string result = line.substr(0, typeBeg);

	// Erase the extracted value from line, including end character.
	line.erase(0, typeBeg + 1);
	return result;
}

template<typename T>
size_t GetArrayLength(T* arr) {
	return sizeof(arr) / sizeof(T*);
}

template<typename T>
size_t GetArrayLength(T arr) {
	return sizeof(arr) / sizeof(T);
}

template<typename T>
void SafeDeleteArray(T** arr) {
	size_t length = GetArrayLength(arr);
	for (size_t i = 0; i < length; ++i) {
		delete arr[i];
	}
	delete[] arr;
}

template<typename T>
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

template<class TKey, class TVal>
void Safe_Delete_Map(std::unordered_map<TKey, TVal>& map)
{
	for (auto i = map.begin(); i != map.end(); i++)
	{
		delete i->second;
	}
	map.clear();
}

inline bool StrContains(const char* str, std::string value) {
	return std::string(str).find(value) != std::string::npos;
}

#include <exception>

// Exception handling for D3D11
namespace DX
{
	// Helper class for COM exceptions
	class com_exception : public std::exception
	{
	public:
		com_exception(HRESULT hr) : result(hr) {}

		const char* what() const noexcept override
		{
			static char s_str[64] = {};
			sprintf_s(s_str, "Failure with HRESULT of %08X",
				static_cast<unsigned int>(result));
			return s_str;
		}

	private:
		HRESULT result;
	};

	// Helper utility converts D3D API failures into exceptions.
	inline void ThrowIfFailed(HRESULT hr)
	{
		if (FAILED(hr))
		{
			printf("%s", com_exception(hr).what());
			throw com_exception(hr);
		}
	}
}