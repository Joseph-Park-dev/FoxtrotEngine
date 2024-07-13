#pragma once
#include <string>
#include <unordered_map>

#include "spine/TextureLoader.h"
#include "spine/spine.h"

#include "FoxtrotEngine/Core/SingletonMacro.h"

class FTTexture;
class FTSpineAnimation;
class FoxtrotRenderer;
class SpineTextureLoader;

class ResourceManager
{
	SINGLETON(ResourceManager);

public:
	void				LoadTexture(FoxtrotRenderer* renderer, const std::string fileName);
	void				UpdateTexture(FoxtrotRenderer* renderer, FTTexture* texture, int channels);
	bool				LoadSpineAnimation(std::string key, std::string atlasPath, std::string skeletonDataPath);

	FTTexture*			GetLoadedTexture(const std::string fileName);
	FTSpineAnimation*	GetLoadedSpineAnimation(std::string key);
	// Load a previously loaded FTTexture to Spine Atlas Page
	void				LoadToSpineTexture(spine::AtlasPage& page, spine::String fileName);

public:
	std::unordered_map<std::string, FTTexture*>& 
		GetTexturesMap() {
		return mMapTextures; 
	}

private:
	SpineTextureLoader* mSpineTextureLoader;

	std::unordered_map<std::string, FTTexture*> mMapTextures;
	std::unordered_map<std::string, FTSpineAnimation*> mMapSpineAnimData;

private:
	FTTexture* FindTexture(const std::string fileName);
	FTSpineAnimation* FindSpineAnimation(const std::string keyName);
};

class SpineTextureLoader : public spine::TextureLoader{
public:
	// For spine animation runtime
	// Called when the atlas loads the texture of a page.
	virtual void load(spine::AtlasPage& page, const spine::String& key) override;

	// Called when the atlas is disposed and itself disposes its atlas pages.
	virtual void unload(void* texture) override;
};