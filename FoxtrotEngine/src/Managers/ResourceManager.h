#pragma once
#include <string>
#include <unordered_map>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <nlohmann/json.hpp>

#include "Core/SingletonMacro.h"
#include "FileSystem/ChunkLoader.h"
#include "FileSystem/ChunkFileKeys.h"

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
	void				UpdateTexture(FTTexture* texture, int channels);

	//void				LoadMeshFromFile(const std::string filePath);
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

public:
	template<typename FTRESOURCE>
	void RemoveResource(UINT key, std::unordered_map<UINT, FTRESOURCE*>& resMap)
	{
		FTRESOURCE* resource = resMap.at(key);
		if (resource)
		{
			delete resource;
			resource = nullptr;
			resMap.erase(key);
		}
	}

private:
	template<typename FTRESOURCE>
	FTRESOURCE* LoadResource(const std::string filePath, std::unordered_map<UINT, FTRESOURCE*>& resMap)
	{
		UINT key = gItemKey;
		std::string fileName = filePath.substr(filePath.rfind("\\") + 1);
		if (!ResourceExists<FTRESOURCE*>(key, filePath, resMap)) {
			printf("Message: Loading FTTexture %s to key %d. \n", filePath.c_str(), key);
			FTRESOURCE* res = new FTRESOURCE;
			res->SetFileName(fileName);
			res->SetRelativePath(filePath);
			resMap.insert(std::make_pair(key, res));
			++gItemKey;
			return res;
		}
		else {
			printf("Warning : Resource %s is already loaded to key %d.\n", filePath.c_str(), key);
		}
	}

	template<typename FTRESOURCE>
	void LoadResource(nlohmann::ordered_json& itemTree, const UINT key, std::unordered_map<UINT, FTRESOURCE*>& resMap)
	{
		FTRESOURCE* resource = new FTRESOURCE;
		resource->LoadProperties(itemTree);
		resMap.insert(std::make_pair(key, resource));
	}

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

private:
	void ProcessTexture(FTTexture* texture);
	void ProcessTextures();

	template<typename FTRESOURCE>
	void SaveResourceInMap(std::ofstream& ofs, const std::unordered_map<UINT, FTRESOURCE*>& resMap) {
		typename std::unordered_map<UINT, FTRESOURCE*>::const_iterator iter;
		for (iter = resMap.begin(); iter != resMap.end(); ++iter) {
			(*iter).second->SaveProperties(ofs);
		}
	}

	//template<typename FTRESOURCE>
	//void LoadResourceToMap(std::ifstream& ifs, std::unordered_map<UINT, FTRESOURCE*>& resMap) {
	//	gItemKey = 0;
	//	for (auto& itemTree : resTree[typeid(FTRESOURCE).name()]) {
	//		UINT key = itemTree["Key"];
	//		LoadResource(itemTree, key, resMap);
	//		if (gItemKey < key)
	//			gItemKey = key;
	//	}
	//	++gItemKey; // Key of the next resource to be imported.
	//}

public:
	void SaveResources(std::ofstream& ofs);
	void LoadResources(std::ifstream& ifs);

#ifdef FOXTROT_EDITOR
public:
	void SaveResources(nlohmann::ordered_json& out);
	void LoadResources(nlohmann::ordered_json& resourceTree);
	void UpdateUI();

private:
	template<typename FTRESOURCE>
	void SaveResourceInMap(nlohmann::ordered_json& out, const std::unordered_map<UINT, FTRESOURCE*>& resMap) {
		typename std::unordered_map<UINT, FTRESOURCE*>::const_iterator iter;
		for (iter = resMap.begin(); iter != resMap.end(); ++iter) {
			(*iter).second->SaveProperties(out[typeid(FTRESOURCE).name()][std::to_string((*iter).first)], (*iter).first);
		}
	}

	template<typename FTRESOURCE>
	void LoadResourceToMap(nlohmann::ordered_json& resTree, std::unordered_map<UINT, FTRESOURCE*>& resMap) {
		gItemKey = 0;
		for (auto& itemTree : resTree[typeid(FTRESOURCE).name()]) {
			UINT key = itemTree["Key"];
			LoadResource(itemTree, key, resMap);
			if (gItemKey < key)
				gItemKey = key;
		}
		++gItemKey; // Key of the next resource to be imported.
	}
#endif // FOXTROT_EDITOR
};