#pragma once
#include <string>
#include <unordered_map>

#include "SingletonMacro.h"

class FTTexture;

class ResourceManager
{
	SINGLETON(ResourceManager);

public:
	void		LoadTexture(const std::string& strKey, const std::string& strRelativePath);
	FTTexture*	GetLoadedTexture(const std::string& strKey);

private:
	std::unordered_map<std::string, FTTexture*> mapTextures;

private:
	FTTexture* FindTexture(const std::string& strKey);
};