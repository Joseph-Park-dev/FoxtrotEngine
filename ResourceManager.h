#pragma once
#include <string>
#include <unordered_map>

#include "SingletonMacro.h"

class FTTexture;
class FoxtrotRenderer;

class ResourceManager
{
	SINGLETON(ResourceManager);

public:
	void		LoadTexture(FoxtrotRenderer* renderer, const std::string fileName);
	FTTexture*	GetLoadedTexture(const std::string fileName);

private:
	std::unordered_map<std::string, FTTexture*> mMapTextures;

private:
	FTTexture* FindTexture(const std::string fileName);
};