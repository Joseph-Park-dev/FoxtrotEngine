#include <unordered_map>

#include "ResourceManager.h"
#include "FTTexture.h"
#include "FTCore.h"
#include "FoxtrotRenderer.h"
#include "TemplateFunctions.h"

void ResourceManager::LoadTexture(FoxtrotRenderer* renderer, const std::string fileName)
{
	FTTexture* ptTex = FindTexture(fileName);
	if (ptTex != nullptr)
		printf("Warning : FTTexture %ls is already loaded.\n", fileName.c_str());
	else
	{
		printf("Message: Loading FTTexture %s\n", fileName.c_str());
		ptTex = new FTTexture;
		ptTex->SetRelativePath(fileName);
		if (!ptTex->CreateTexture(renderer))
			printf("Error: ResourceManager::LoadTexture() -> CreateTexture failed. \n");
		mMapTextures.insert(std::make_pair(fileName, ptTex));
	}
}

void ResourceManager::UpdateTexture(FoxtrotRenderer* renderer, FTTexture* texture, int channels)
{
	if (texture != nullptr)
	{
		if (!texture->ReleaseTexture())
		{
			LogString("Error: ResourceManager::UpdateTexture() : ReleaseTexture failed");
			return;
		}
		printf("Message: Loading FTTexture %s\n", texture->GetRelativePath());
		if (!texture->CreateTexture(renderer))
			printf("Error: ResourceManager::LoadTexture() -> UpdateTexture failed. \n");
	}
	else
		printf("Warning : FTTexture %s is not loaded. Aborting update...\n");
}

FTTexture* ResourceManager::GetLoadedTexture(const std::string fileName)
{
	FTTexture* ptTex = FindTexture(fileName);
	if (ptTex != nullptr)
		return ptTex;
	else
	{
		printf("Error: Unable to find FTTexture with fileName; %s\n", fileName.c_str());
		return nullptr;
	}
}

FTTexture* ResourceManager::FindTexture(const std::string fileName)
{
	auto iter = mMapTextures.find(fileName);
	if (iter != mMapTextures.end())
		return iter->second;
	else
	{
		printf("Error: Cannot find texture %s\n", fileName.c_str());
		return nullptr;
	}
}

ResourceManager::~ResourceManager()
{
	std::unordered_map<std::string, FTTexture*>::iterator iter = mMapTextures.begin();
	for (; iter != mMapTextures.end(); iter++)
	{
		delete iter->second;
	}
}

ResourceManager::ResourceManager()
{

}