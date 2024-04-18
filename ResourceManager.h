#pragma once
#include <string>
#include <unordered_map>

#include "SingletonMacro.h"

class FTTexture;

class ResourceManager
{
	SINGLETON(ResourceManager);

public:
	void		LoadTexture(const std::wstring& strKey, const std::wstring& strRelativePath);
	FTTexture*	GetLoadedTexture(const std::wstring& strKey);
	FTTexture*	CreateTexture(const std::wstring& strKey, UINT width, UINT height);

private:
	std::unordered_map<std::wstring, FTTexture*> mapTextures;

private:
	FTTexture* FindTexture(const std::wstring& strKey);
};