#pragma once
#include <string>
#include <unordered_map>
#include <SDL2/SDL.h>

#include "SingletonMacro.h"

class Texture;

class ResourceManager
{
	SINGLETON(ResourceManager);

public:
	void		LoadTexture(const std::wstring& strKey, const std::wstring& strRelativePath);
	Texture*	GetLoadedTexture(const std::wstring& strKey);
	Texture*	CreateTexture(const std::wstring& strKey, Uint32 width, Uint32 height);

private:
	std::unordered_map<std::wstring, Texture*> mapTextures;

private:
	Texture* FindTexture(const std::wstring& strKey);
};