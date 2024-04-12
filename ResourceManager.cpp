#include <unordered_map>

#include "ResourceManager.h"
#include "Texture.h"
#include "CCore.h"

void ResourceManager::LoadTexture(const std::wstring& strKey, const std::wstring& strRelativePath)
{
	Texture* ptTex = FindTexture(strKey);
	if (ptTex != nullptr)
	{
		SDL_Log("Warning : Texture %ls is already loaded from %ls", strKey.c_str(), strRelativePath.c_str());
	}
	else
	{
		SDL_Log("Message: Loading Texture %ls from %ls...", strKey.c_str(), strRelativePath.c_str());
		ptTex = new Texture;
		ptTex->SetKey(strKey);
		ptTex->SetRelativePath(strRelativePath);
		SDL_Surface* surf = ptTex->LoadSurface();
		ptTex->CreateTextureFromSurface(surf);
		mapTextures.insert(std::make_pair(strKey, ptTex));
	}
}

Texture* ResourceManager::GetLoadedTexture(const std::wstring& strKey)
{
	Texture* ptTex = FindTexture(strKey);
	if (ptTex != nullptr)
	{
		return ptTex;
	}
	else
	{
		SDL_Log("Error: Unable to find Texture with strKey; %ls", strKey.c_str());
		return nullptr;
	}
}

Texture* ResourceManager::CreateTexture(const std::wstring& strKey, Uint32 width, Uint32 height)
{
	Texture* ptTex = FindTexture(strKey);
	if (ptTex != nullptr)
		return ptTex;
	else
	{
		SDL_Log("Failed to create texture %ls", strKey.c_str());
		return nullptr;
	}
}

Texture* ResourceManager::FindTexture(const std::wstring& strKey)
{
	auto iter = mapTextures.find(strKey);
	if (iter != mapTextures.end())
	{
		return iter->second;
	}
	else
	{
		SDL_Log("Error: Cannot find texture %ls", strKey.c_str());
		return nullptr;
	}
}

ResourceManager::~ResourceManager()
{
	std::unordered_map<std::wstring, Texture*>::iterator iter = mapTextures.begin();
	for (; iter != mapTextures.end(); iter++)
	{
		delete iter->second;
	}
}

ResourceManager::ResourceManager()
{

}