#pragma once
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <iterator>
#include <stdlib.h>
#include <fstream>
#include <string>

#include "Math.h"

//static std::string ToString(const wchar_t* text)
//{
//	size_t i = 0;
//	size_t size = std::wcslen(text);
//	char* str = new char[size];
//	if (size > 0) {
//		wcstombs_s(&i, str, size, text, _TRUNCATE);
//	}
//	return str;
//}

void LogInt(int val)
{
	printf("int : %d", val);
}

void LogInt(std::string message, int val)
{
	std::string msg = message + std::to_string(val);
	printf(msg.c_str());
}

void LogFloat(float val)
{
	printf("float : %f", val);
}

void LogFloat(std::string message, float val)
{
	std::string msg = message + std::to_string(val);
	printf(msg.c_str());
}

void LogVector2(Vector2 val)
{
	printf("X : %f Y : %f", val.x, val.y);
}

void LogVector2(std::string message, Vector2 val)
{
	printf("%s, X : %f Y : %f", message.c_str(), val.x, val.y);
}

void LogRect(SDL_Rect rect)
{
	printf("Pos X : %d, Pos Y : %d, Width : %d, Height : %d", rect.x, rect.y, rect.w, rect.h);
}

void LogBool(bool val)
{
	if (val)
		printf("Bool Log : True");
	else
		printf("Bool Log : False");
}

void LogString(std::wstring val)
{
	printf("%ls", val.c_str());
}

void LogString(std::string val)
{
	printf("%s", val.c_str());
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
	char* str = new char[size];
	if (0 < size) 
	{
		wcstombs_s(&i, str, size, text, _TRUNCATE);
		return str;
	}
	return nullptr;
}

//b2Vec2 ToB2Vec2(Vector2 vec)
//{
//	return b2Vec2(vec.x, vec.y);
//}

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

