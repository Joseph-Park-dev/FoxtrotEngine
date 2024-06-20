#include <unordered_map>

#include <spine/Atlas.h>

#include "ResourceManager.h"
#include "FTTexture.h"
#include <FTSpineTexture.h>
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

void ResourceManager::LoadSpineTexture(FoxtrotRenderer* renderer, spine::String fileName)
{
	FTTexture* ptTex = FindTexture(fileName.buffer());
	if (ptTex != nullptr)
		printf("Warning : FTSpineTexture %ls is already loaded.\n", fileName.buffer());
	else {
		printf("Message: Loading FTTexture %s\n", fileName.buffer());
		FTSpineTexture* spineTex = new FTSpineTexture;
		std::string fileNameStr = fileName.buffer();
		spineTex->SetRelativePath(fileNameStr);
		if (!spineTex->CreateTexture(renderer)) {
			printf("Error: ResourceManager::LoadTexture() -> CreateTexture failed. \n");
			return;
		}
		load(*spineTex->GetAtlasPage(), spine::String(spineTex->GetRelativePath().c_str()));
		mMapTextures.insert(std::make_pair(fileNameStr, ptTex));
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

FTSpineTexture* ResourceManager::GetLoadedSpineTexture(spine::String fileName)
{
	std::string fileNameStr = std::string(fileName.buffer());
	FTTexture* ptTex = FindTexture(fileNameStr);
	if (ptTex != nullptr)
		return dynamic_cast<FTSpineTexture*>(ptTex);
	else
	{
		printf("Error: Unable to find FTTexture with fileName; %s\n", fileNameStr.c_str());
		return nullptr;
	}
}

// Loads spine atlas 
void ResourceManager::load(spine::AtlasPage& page, const spine::String& path)
{
	FTTexture* tex = ResourceManager::GetInstance()->GetLoadedTexture(path.buffer());
	DirectX::ScratchImage* scratchImg = tex->GetScratchImage();
	if (!scratchImg) return;
	//page.texture = tex->GetResourceView();
	page.texture = dynamic_cast<FTSpineTexture*>(tex);
	page.width = scratchImg->GetImages()->width;
	page.height = scratchImg->GetImages()->height;
	page.texturePath = path;

	/* if (page.magFilter == TextureFilter_Linear) texture->setSmooth(true);
	 if (page.uWrap == TextureWrap_Repeat && page.vWrap == TextureWrap_Repeat) texture->setRepeated(true);*/
}

void ResourceManager::unload(void* texture)
{
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

spine::SpineExtension* getDefaultExtension() {
	return new spine::DefaultSpineExtension();
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