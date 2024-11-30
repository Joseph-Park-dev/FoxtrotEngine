#pragma once
#include <string>
#include <unordered_map>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "Core/SingletonMacro.h"
#include "FileSystem/ChunkLoader.h"
#include "FileSystem/ChunkFileKeys.h"
#include "ResourceSystem/FTPremade.h"
#include "Debugging/DebugMemAlloc.h"

class FTTexture;
class FTSpineAnimation;
class FTBasicMeshGroup;
class FoxtrotRenderer;
class SpineTextureLoader;
class MeshData;
class FTTileMap;

enum class ResType
{
	UNSUPPORTED,
	FTTEXTURE,
	FTTILEMAP,
	FTPREMADE
};

class ResourceManager
{
	SINGLETON(ResourceManager);

public:
	void					Initialize(FoxtrotRenderer* renderer);
	void					DeleteAll();

	void					SaveResources(std::ofstream& ofs);
	void					LoadResources(std::ifstream& ifs, FTCore* ftCoreInst);

public:
	void					LoadBasicMesh(MeshData meshData);
	UINT					LoadBasicMesh(std::vector<MeshData> meshData);
	void					LoadTileMap(const std::string filePath);

	std::shared_ptr<FTTexture>& GetLoadedTexture(const UINT key);
	std::shared_ptr<FTTileMap>& GetLoadedTileMap(const UINT key);
	std::shared_ptr<FTPremade>& GetLoadedPremade(const UINT key);
	std::shared_ptr<FTPremade>& GetLoadedPremade(std::string&& fileName);
	std::vector<MeshData>&		GetLoadedMeshes	(const UINT key);
	MeshData&					GetLoadedPrimitive(const UINT key);

	void					RemoveLoadedMeshes(const UINT key);

public:
	std::unordered_map<UINT, std::shared_ptr<FTTexture>>& GetTexturesMap();
	// I know the name feels so funny...
	std::unordered_map<UINT, std::shared_ptr<FTTileMap>>& GetTileMapsMap();
	
	std::string& GetPathToAsset();
	void		 SetPathToAsset(std::string&& projectPath);

public:
	static	UINT		gItemKey;
			std::string mPathToAsset;

private:
	std::unordered_map<UINT, std::shared_ptr<FTTexture>>	mMapTextures;
	std::unordered_map<UINT, std::shared_ptr<FTTileMap>>	mMapTileMaps;
	std::unordered_map<UINT, std::shared_ptr<FTPremade>>	mMapPremades;
	std::unordered_map<UINT, std::vector<MeshData>>			mMapMeshes;
	std::unordered_map<UINT, MeshData>						mMapPrimitives;

	FoxtrotRenderer* mRenderer; // For Loading FTTextures

public:
	template<typename FTRESOURCE>
	void SaveResourceToChunk(const std::ofstream& ofs, const std::unordered_map<UINT, std::shared_ptr<FTRESOURCE>>& resMap)
	{
		typename std::unordered_map<UINT, FTRESOURCE*>::const_iterator iter;
		for (iter = resMap.begin(); iter != resMap.end(); ++iter) {
			if ((*iter).second->IsReferenced())
				(*iter).second->SaveProperties(ofs, (*iter).first);
		}
	}

	template<typename FTRESOURCE>
	void LoadResourceFromChunk(const std::ifstream& ifs, const std::unordered_map<UINT, std::shared_ptr<FTRESOURCE>>& resMap, size_t& resCount)
	{
		gItemKey += resCount;
		if (0 < resCount)
		{
			while (0 < resCount)
			{
				LoadResource(ifs, resMap);
				--resCount; // Key of the next resource to be imported.
			}
			// Subtract the number of resources loaded.
		}
	}

	template<typename FTRESOURCE>
	void ClearMap(std::unordered_map<UINT, FTRESOURCE*>& resMap) 
	{
		auto iter = resMap.begin();
		for (; iter != resMap.end(); ++iter)
		{
			delete (*iter).second;
			(*iter).second = nullptr;
		}
		resMap.clear();
	}

	template<typename FTRESOURCE>
	void RemoveResource(const UINT key, const std::unordered_map<UINT, std::shared_ptr<FTRESOURCE>>& resMap)
	{
		std::shared_ptr<FTRESOURCE>& resource = resMap.at(key);
		if (resource)
			resMap.erase(key);
		else
			printf("ERROR: ResourceManager::RemoveResource()->key %d does not exist", key);
	}

private:
	void ProcessTexture(std::shared_ptr<FTTexture>& texture);
	void ProcessTextures();
	void ProcessPremades();

private:
	template<typename FTRESOURCE>
	std::shared_ptr<FTRESOURCE> LoadResource(const std::string filePath, const std::unordered_map<UINT, std::shared_ptr<FTRESOURCE>>& resMap)
	{
		UINT key = gItemKey;
		std::string fileName = filePath.substr(filePath.rfind("\\") + 1);
		if (!ResourceExists<FTRESOURCE*>(key, filePath, resMap)) {
			printf("Message: Loading FTTexture %s to key %d. \n", filePath.c_str(), key);
			std::shared_ptr<FTRESOURCE> res = std::make_shared<FTRESOURCE>();
			res->SetFileName(fileName);
			res->SetRelativePath(filePath);
			resMap.insert(std::make_pair(key, res));
			++gItemKey;
			return res;
		}
		else {
			printf("Warning : Resource %s is already loaded to key %d.\n", filePath.c_str(), key);
			return nullptr;
		}
	}

	template<typename FTRESOURCE>
	void LoadResource(std::ifstream& ifs, std::unordered_map<UINT, FTRESOURCE*>& resMap)
	{
		std::shared_ptr<FTRESOURCE> resource = std::make_shared<FTRESOURCE>();
		UINT key = resource->LoadProperties(ifs);
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

#ifdef FOXTROT_EDITOR
public:
	void LoadAllResourcesInAsset();
	void LoadResByType(std::string& fileName);

public:
	void UpdateUI();

private:
	ResType GetResType(std::string& fileName);

#endif // FOXTROT_EDITOR
};