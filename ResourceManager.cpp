#include <unordered_map>

#include "ResourceManager.h"
#include "FTTexture.h"
#include "CCore.h"

void ResourceManager::LoadTexture(const std::string& strKey, const std::string& fileName)
{
	FTTexture* ptTex = FindTexture(strKey);
	if (ptTex != nullptr)
		printf("Warning : FTTexture %ls is already loaded from %ls\n", strKey.c_str(), fileName.c_str());
	else
	{
		printf("Message: Loading FTTexture %ls from %ls...\n", strKey.c_str(), fileName.c_str());
		ptTex = new FTTexture;
		ptTex->SetKey(strKey);
		ptTex->SetRelativePath(fileName);
		mapTextures.insert(std::make_pair(strKey, ptTex));
	}
}

FTTexture* ResourceManager::GetLoadedTexture(const std::string& strKey)
{
	FTTexture* ptTex = FindTexture(strKey);
	if (ptTex != nullptr)
		return ptTex;
	else
	{
		printf("Error: Unable to find FTTexture with strKey; %ls\n", strKey.c_str());
		return nullptr;
	}
}

FTTexture* ResourceManager::FindTexture(const std::string& strKey)
{
	auto iter = mapTextures.find(strKey);
	if (iter != mapTextures.end())
		return iter->second;
	else
	{
		printf("Error: Cannot find texture %ls\n", strKey.c_str());
		return nullptr;
	}
}

ResourceManager::~ResourceManager()
{
	std::unordered_map<std::string, FTTexture*>::iterator iter = mapTextures.begin();
	for (; iter != mapTextures.end(); iter++)
	{
		delete iter->second;
	}
}

ResourceManager::ResourceManager()
{

}