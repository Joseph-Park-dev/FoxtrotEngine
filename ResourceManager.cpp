#include <unordered_map>

#include "ResourceManager.h"
#include "FTTexture.h"
#include "CCore.h"

void ResourceManager::LoadTexture(const std::wstring& strKey, const std::wstring& strRelativePath)
{
	FTTexture* ptTex = FindTexture(strKey);
	if (ptTex != nullptr)
	{
		printf("Warning : FTTexture %ls is already loaded from %ls\n", strKey.c_str(), strRelativePath.c_str());
	}
	else
	{
		printf("Message: Loading FTTexture %ls from %ls...\n", strKey.c_str(), strRelativePath.c_str());
		ptTex = new FTTexture;
		ptTex->SetKey(strKey);
		ptTex->SetRelativePath(strRelativePath);
		/*
			TEXTURE
			Load CODE HERE
			(Alternative to 
			SDL_Surface* surf = ptTex->LoadSurface();)
		*/
		
		//ptTex->CreateTextureFromSurface(surf);
		mapTextures.insert(std::make_pair(strKey, ptTex));
	}
}

FTTexture* ResourceManager::GetLoadedTexture(const std::wstring& strKey)
{
	FTTexture* ptTex = FindTexture(strKey);
	if (ptTex != nullptr)
	{
		return ptTex;
	}
	else
	{
		printf("Error: Unable to find FTTexture with strKey; %ls\n", strKey.c_str());
		return nullptr;
	}
}

FTTexture* ResourceManager::CreateTexture(const std::wstring& strKey, UINT width, UINT height)
{
	FTTexture* ptTex = FindTexture(strKey);
	if (ptTex != nullptr)
		return ptTex;
	else
	{
		printf("Failed to create texture %ls", strKey.c_str());
		return nullptr;
	}
}

FTTexture* ResourceManager::FindTexture(const std::wstring& strKey)
{
	auto iter = mapTextures.find(strKey);
	if (iter != mapTextures.end())
	{
		return iter->second;
	}
	else
	{
		printf("Error: Cannot find texture %ls\n", strKey.c_str());
		return nullptr;
	}
}

ResourceManager::~ResourceManager()
{
	std::unordered_map<std::wstring, FTTexture*>::iterator iter = mapTextures.begin();
	for (; iter != mapTextures.end(); iter++)
	{
		delete iter->second;
	}
}

ResourceManager::ResourceManager()
{

}