#pragma once
#include <string>
#include <unordered_map>

#include "FoxtrotEngine/Core/SingletonMacro.h"

class FTTexture;
class FTSpineAnimation;
class FoxtrotRenderer;

class ResourceManager
{
	SINGLETON(ResourceManager);

public:
	void				LoadTexture(FoxtrotRenderer* renderer, const std::string fileName);
	void				UpdateTexture(FoxtrotRenderer* renderer, FTTexture* texture, int channels);
	FTTexture*			GetLoadedTexture(const std::string fileName);

	std::unordered_map<std::string, FTTexture*>& 
		GetTexturesMap() {
		return mMapTextures; 
	}

	//void				LoadSpineTexture(FoxtrotRenderer* renderer, spine::String fileName);
	//FTSpineAnimation*	GetLoadedSpineTexture(spine::String fileName);

private:
	//void		load(spine::AtlasPage& page, const spine::String& path) override;
	//void		unload(void* texture) override;

private:
	std::unordered_map<std::string, FTTexture*> mMapTextures;

private:
	FTTexture* FindTexture(const std::string fileName);
};