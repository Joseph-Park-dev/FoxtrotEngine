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
	void						Initialize(FoxtrotRenderer* renderer);
	void						DeleteAll();
								
	void						SaveResources(std::ofstream& ofs);
	void						LoadResources(std::ifstream& ifs, FTCore* ftCoreInst);

public:
	FTTexture*					GetLoadedTexture(const UINT key);
	FTTileMap*					GetLoadedTileMap(const UINT key);
	FTPremade*					GetLoadedPremade(const UINT key);
	FTPremade*					GetLoadedPremade(std::string&& fileName);
	std::vector<MeshData>&		GetLoadedMeshes	(const UINT key);
	MeshData&					GetLoadedPrimitive(const UINT key);
	void						RemoveLoadedMeshes(const UINT key);

public:
	std::unordered_map<UINT, FTTexture*>& GetTexturesMap();
	// I know the name feels so funny...
	std::unordered_map<UINT, FTTileMap*>& GetTileMapsMap();
	
	std::string& GetPathToAsset();
	void		 SetPathToAsset(std::string&& projectPath);

public:
	static	UINT		gItemKey;
			std::string mPathToAsset;

private:
	std::unordered_map<UINT, FTTexture*>			mMapTextures;
	std::unordered_map<UINT, FTTileMap*>			mMapTileMaps;
	std::unordered_map<UINT, FTPremade*>			mMapPremades;
	std::unordered_map<UINT, std::vector<MeshData>>	mMapMeshes;
	std::unordered_map<UINT, MeshData>				mMapPrimitives;

	FoxtrotRenderer* mRenderer; // For Loading FTTextures

/// <Chunk IO> -------------------------------------
/// Template member functions for saving/loading resources to/from chunk.
/// </Chunk IO>
public:
	template<typename FTRESOURCE>
	void SaveResourceToChunk(std::ofstream& ofs, std::unordered_map<UINT, FTRESOURCE*>& resMap)
	{
		typename std::unordered_map<UINT, FTRESOURCE*>::const_iterator iter;
		for (iter = resMap.begin(); iter != resMap.end(); ++iter) {
			if (0 < (*iter).second->GetRefCount())
				(*iter).second->SaveProperties(ofs, (*iter).first);
		}
	}

	template<typename FTRESOURCE>
	void LoadResourceFromChunk(std::ifstream& ifs, std::unordered_map<UINT, FTRESOURCE*>& resMap, size_t& resCount)
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

private:
	template<typename FTRESOURCE>
	void LoadResource(std::ifstream& ifs, std::unordered_map<UINT, FTRESOURCE*>& resMap)
	{
		FTRESOURCE* resource = new FTRESOURCE;
		UINT key = resource->LoadProperties(ifs);
		resMap.insert(std::make_pair(key, resource));
	}

/// <Creating New Resources> -------------------------------------
/// Template member functions for creating new resources & adding to resource map.
/// </Creating New Resources>
private:
	template<typename FTRESOURCE>
	FTRESOURCE* LoadResource(std::string& filePath, std::unordered_map<UINT, FTRESOURCE*>& resMap)
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
			return nullptr;
		}
	}

/// <Removing Resources> -------------------------------------
/// Template member functions for removing the loaded resources.
/// </Removing Resources>
public:
	template<typename FTRESOURCE>
	void ClearMap(std::unordered_map<UINT, FTRESOURCE*>& resMap) 
	{
		auto iter = resMap.begin();
		for (; iter != resMap.end(); ++iter)
		{
			if ((*iter).second != nullptr)
			{
				delete (*iter).second;
				(*iter).second = nullptr;
			}
		}
		resMap.clear();
	}

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
		else
			printf("ERROR: ResourceManager::RemoveResource()->key %d does not exist", key);
	}

/// <Processing Resources> -------------------------------------
/// Template member functions for processing newly loaded resources. 
/// </Processing Resources>
private:
	void ProcessTexture(FTTexture* texture);
	void ProcessTextures();
	void ProcessPremades();

/// <Validating Resources> -------------------------------------
/// Template member functions for validating the keys & resources.
/// </Validating Resources>
private:
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

/// <Editor Related Features> -------------------------------------
/// This part will be filtered in game, only running on Foxtrot Editor.
/// </Editor Related Features>
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