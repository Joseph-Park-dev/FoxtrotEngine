#pragma once
#include <string>
#include <unordered_map>

#include "spine/TextureLoader.h"
#include "spine/spine.h"

#include "FoxtrotEngine/Core/SingletonMacro.h"

class FTTexture;
class FTSpineAnimation;
class FTBasicMeshGroup;
class FoxtrotRenderer;
class SpineTextureLoader;
class MeshData;

class ResourceManager
{
	SINGLETON(ResourceManager);

public:
	void				Initialize(FoxtrotRenderer* renderer);

public:
	void				LoadTexture(const std::string key, const std::string filePath);
	void				UpdateTexture(FTTexture* texture, int channels);
	//bool				LoadSpineAnimation(std::string key, std::string atlasPath, std::string skeletonDataPath);

	void				LoadMeshFromFile(const std::string key, const std::string filePath);
	void				LoadBasicMesh(const std::string key, MeshData meshData);
	void				LoadBasicMesh(const std::string key, std::vector<MeshData> meshData);

	FTTexture*			GetLoadedTexture(const std::string key);
	// FTSpineAnimation*	GetLoadedSpineAnimation(std::string key);
	// Load a previously loaded FTTexture to Spine Atlas Page
	// void				LoadToSpineTexture(spine::AtlasPage& page, spine::String fileName);
	std::vector<MeshData>&	GetLoadedMeshes(const std::string key);

	void				RemoveLoadedMeshes(const std::string key);

public:
	std::unordered_map<std::string, FTTexture*>& 
		GetTexturesMap() {
		return mMapTextures; 
	}

private:
	SpineTextureLoader* mSpineTextureLoader;

	std::unordered_map<std::string, FTTexture*> mMapTextures;
	//std::unordered_map<std::string, FTSpineAnimation*> mMapSpineAnimData;
	std::unordered_map<std::string, std::vector<MeshData>> mMapMeshes;

	std::string mPathToAsset;
	FoxtrotRenderer* mRenderer; // For Loading FTTextures

private:
	//FTSpineAnimation*	FindSpineAnimation(const std::string keyName);

	template<typename FTRESOURCE>
	bool ResourceExists(const std::string key, const std::string path, const std::unordered_map<std::string, FTRESOURCE>& resMap) {
		if (0 < resMap.size()) {
			KeyExists(key, resMap);
			auto iter = resMap.begin();
			for (; iter != resMap.end(); ++iter) {
				if ((*iter).second->GetRelativePath() == path)
				{
					printf("Error: ResourceManager::ResourceExists() -> Resource with path %s exists\n", path.c_str());
					return true;
				}
			}
			return false;
		}
		return false;
	}

	template<typename FTRESOURCE>
	bool KeyExists(const std::string key, const std::unordered_map<std::string, FTRESOURCE>& resMap) {
		if (resMap.find(key) != resMap.end()) {
			printf("Error: ResourceManager::ResourceExists() -> Resource with key %s exists\n", key.c_str());
			return true;
		}
		return false;
	}

public:
	void SaveResources(nlohmann::ordered_json& out);
};

class SpineTextureLoader : public spine::TextureLoader{
public:
	// For spine animation runtime
	// Called when the atlas loads the texture of a page.
	virtual void load(spine::AtlasPage& page, const spine::String& key) override;

	// Called when the atlas is disposed and itself disposes its atlas pages.
	virtual void unload(void* texture) override;
};