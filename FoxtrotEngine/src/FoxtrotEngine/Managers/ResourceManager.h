#pragma once
#include <string>
#include <unordered_map>
#include <windef.h>

#include "spine/TextureLoader.h"
#include "spine/spine.h"

#include "FoxtrotEngine/Core/SingletonMacro.h"

class FTTexture;
class FTSpineAnimation;
class FTBasicMeshGroup;
class FoxtrotRenderer;
class SpineTextureLoader;
class MeshData;
class FTTileMap;

class ResourceManager
{
	SINGLETON(ResourceManager);

public:
	void				Initialize(FoxtrotRenderer* renderer);

public:
	void				LoadTexture(const std::string filePath);
	void				UpdateTexture(FTTexture* texture, int channels);
	//bool				LoadSpineAnimation(std::string key, std::string atlasPath, std::string skeletonDataPath);

	void				LoadMeshFromFile(const std::string filePath);
	void				LoadBasicMesh(MeshData meshData);
	void				LoadBasicMesh(std::vector<MeshData> meshData);

	void				LoadTileMap(const std::string filePath);

	FTTexture*			GetLoadedTexture(UINT key);
	// FTSpineAnimation*	GetLoadedSpineAnimation(std::string key);
	// Load a previously loaded FTTexture to Spine Atlas Page
	// void				LoadToSpineTexture(spine::AtlasPage& page, spine::String fileName);
	std::vector<MeshData>&	GetLoadedMeshes(UINT key);
	FTTileMap*			GetLoadedTileMap(UINT key);

	void				RemoveLoadedMeshes(UINT key);

public:
	std::unordered_map<UINT, FTTexture*>& 
		GetTexturesMap() {
		return mMapTextures; 
	}

public:
	static UINT gItemKey;

private:
	SpineTextureLoader* mSpineTextureLoader;

	std::unordered_map<UINT, FTTexture*>			mMapTextures;
	std::unordered_map<UINT, std::vector<MeshData>>	mMapMeshes;
	std::unordered_map<UINT, FTTileMap*>			mMapTileMaps;

	std::unordered_map<UINT, MeshData>				mMapPrimitives;

	std::string mPathToAsset;
	FoxtrotRenderer* mRenderer; // For Loading FTTextures

private:
	//FTSpineAnimation*	FindSpineAnimation(const std::string keyName);
	template<typename FTRESOURCE>
	bool KeyExists(const UINT key, const std::unordered_map<UINT, FTRESOURCE>& resMap) {
		if (resMap.find(key) != resMap.end()) {
			printf("Error: ResourceManager::ResourceExists() -> Resource with key %d exists\n", key);
			return true;
		}
		return false;
	}

	template<typename FTRESOURCE>
	bool ResourceExists(const UINT key, const std::string path, const std::unordered_map<UINT, FTRESOURCE>& resMap) {
		if (0 < resMap.size()) {
			if (!KeyExists(key, resMap)) {
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
		return false;
	}

#ifdef _DEBUG
public:
	void SaveResources(nlohmann::ordered_json& out);

	void UpdateUI();
#endif
};

class SpineTextureLoader : public spine::TextureLoader{
public:
	// For spine animation runtime
	// Called when the atlas loads the texture of a page.
	virtual void load(spine::AtlasPage& page, const spine::String& key) override;

	// Called when the atlas is disposed and itself disposes its atlas pages.
	virtual void unload(void* texture) override;
};