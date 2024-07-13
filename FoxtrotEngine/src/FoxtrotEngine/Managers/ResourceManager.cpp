#include <unordered_map>

#include "FoxtrotEngine/Managers/ResourceManager.h"
#include "FoxtrotEngine/ResourceSystem/FTTexture.h"
#include "FoxtrotEngine/ResourceSystem/FTSpineAnimation.h"
#include "FoxtrotEngine/Core/FTCore.h"
#include "FoxtrotEngine/Core/TemplateFunctions.h"
#include "FoxtrotEngine/Renderer/FoxtrotRenderer.h"
#include "spine/Atlas.h"

void ResourceManager::LoadTexture(FoxtrotRenderer* renderer, const std::string fileName)
{
	FTTexture* ptTex = FindTexture(fileName);
	if (ptTex != nullptr)
		printf("Warning : FTTexture %s is already loaded.\n", fileName.c_str());
	else
	{
		printf("Message: Loading FTTexture %s\n", fileName.c_str());
		ptTex = new FTTexture;
		ptTex->SetRelativePath(fileName);
		if (!ptTex->CreateTexture(renderer, fileName))
			printf("Error: ResourceManager::LoadTexture() -> CreateTexture failed. \n");
		mMapTextures.insert(std::make_pair(fileName, ptTex));
	}
}

//void ResourceManager::LoadSpineTexture(FoxtrotRenderer* renderer, spine::String fileName)
//{
//	FTTexture* ptTex = FindTexture(fileName.buffer());
//	if (ptTex != nullptr)
//		printf("Warning : FTSpineTexture %ls is already loaded.\n", fileName.buffer());
//	else {
//		printf("Message: Loading FTTexture %s\n", fileName.buffer());
//		FTSpineTexture* spineTex = new FTSpineTexture;
//		std::string fileNameStr = fileName.buffer();
//		spineTex->SetRelativePath(fileNameStr);
//		if (!spineTex->CreateTexture(renderer)) {
//			printf("Error: ResourceManager::LoadTexture() -> CreateTexture failed. \n");
//			return;
//		}
//		load(*spineTex->GetAtlasPage(), spine::String(spineTex->GetRelativePath().c_str()));
//		mMapTextures.insert(std::make_pair(fileNameStr, ptTex));
//	}
//}

void ResourceManager::UpdateTexture(FoxtrotRenderer* renderer, FTTexture* texture, int channels)
{
	if (texture != nullptr)
	{
		if (!texture->ReleaseTexture())
		{
			LogString("Error: ResourceManager::UpdateTexture() : ReleaseTexture failed");
			return;
		}
		printf("Message: Loading FTTexture %s\n", texture->GetRelativePath().c_str());
		//if (!texture->CreateTexture(renderer))
		//	printf("Error: ResourceManager::LoadTexture() -> UpdateTexture failed. \n");
	}
	else
		printf("Warning : FTTexture %s is not loaded. Aborting update...\n", texture->GetRelativePath().c_str());
}

bool ResourceManager::LoadSpineAnimation(std::string key, std::string atlasPath, std::string skeletonDataPath)
{
	FTSpineAnimation* spineAnim = new FTSpineAnimation;
	if (!spineAnim->CreateAtlas(atlasPath, mSpineTextureLoader)) {
		LogString("ResourceManager::LoadSpineAnimation()-> CreateAtlas() Failed.");
		return false;
	};
	if (!spineAnim->CreateSkeletonDataJSON(skeletonDataPath)) {
		LogString("ResourceManager::LoadSpineAnimation()-> CreateSkeletonData() Failed.");
		return false;
	}
	if (!spineAnim->CreateAnimationStateData()) {
		LogString("ResourceManager::LoadSpineAnimation()-> CreateAnimationStateData() Failed.");
		return false;
	}
	mMapSpineAnimData.insert(std::make_pair(key, spineAnim));
	return true;
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

FTSpineAnimation* ResourceManager::GetLoadedSpineAnimation(std::string key)
{
	FTSpineAnimation* ptAnim = FindSpineAnimation(key);
	if (ptAnim != nullptr)
		return ptAnim;
	else
	{
		printf("Error: Unable to find FTTexture with keyName; %s\n", key.c_str());
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

FTSpineAnimation* ResourceManager::FindSpineAnimation(const std::string keyName)
{
	auto iter = mMapSpineAnimData.find(keyName);
	if (iter != mMapSpineAnimData.end())
		return iter->second;
	else
	{
		printf("Error: Cannot find SpineAnimation %s\n", keyName.c_str());
		return nullptr;
	}
}

//spine::SpineExtension* getDefaultExtension() {
//	return new spine::DefaultSpineExtension();
//}

ResourceManager::~ResourceManager()
{
	std::unordered_map<std::string, FTTexture*>::iterator iter = mMapTextures.begin();
	for (; iter != mMapTextures.end(); iter++)
	{
		delete iter->second;
	}
}

ResourceManager::ResourceManager()
	: mSpineTextureLoader(new SpineTextureLoader)
{

}

void ResourceManager::LoadToSpineTexture(spine::AtlasPage& page, spine::String fileName)
{
	mSpineTextureLoader->load(page, fileName);
}

void SpineTextureLoader::load(spine::AtlasPage& page, const spine::String& path)
{
	FTTexture* tex = ResourceManager::GetInstance()->GetLoadedTexture(path.buffer());
	if (tex == nullptr)
		LogString("SpineTextureLoader::Load() -> Failed to load texture");
	page.texture = tex;
}

void SpineTextureLoader::unload(void* texture)
{
}
