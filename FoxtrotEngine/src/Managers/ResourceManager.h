// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// A manager that saves/loads FTResources referred in a .Chunk file.
/// 
/// In the Foxtrot Editor, this loads all supported resources in the 
/// "Asset" folder, and saves the FTResources that are referred in a
/// .Chunk file or used in a Scene.
/// </summary>

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

#ifdef FOXTROT_EDITOR
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/FileDialog/ImGuiFileDialog.h"
#include "imgui/FileDialog/ImGuiFileDialogConfig.h"
#include <imgui.h>
#endif // FOXTROT_EDITOR

class FTTexture;
class FTSpriteAnimation;
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
	FTTexture*					GetLoadedTexture(const UINT mItemKey);
	FTTileMap*					GetLoadedTileMap(const UINT mItemKey);
	FTPremade*					GetLoadedPremade(const UINT mItemKey);
	FTPremade*					GetLoadedPremade(std::string&& fileName);
	std::vector<MeshData>&		GetLoadedMeshes	(const UINT mItemKey);
	MeshData&					GetLoadedPrimitive(const UINT mItemKey);
	void						RemoveLoadedMeshes(const UINT mItemKey);
	FTSpriteAnimation*			GetLoadedSpriteAnim(const UINT mItemKey);


public:
	std::unordered_map<UINT, FTTexture*>& GetTexturesMap();
	// I know the name feels so funny...
	std::unordered_map<UINT, FTTileMap*>& GetTileMapsMap();
	std::unordered_map<UINT, FTSpriteAnimation*>& GetSpriteAnimMap();
	
	std::string& GetPathToAsset();
	void		 SetPathToAsset(std::string&& projectPath);

private:
	UINT				mItemKey;
	std::string			mPathToAsset;
	FoxtrotRenderer*	mRenderer;		// For Loading FTTextures

private:
	std::unordered_map<UINT, FTTexture*>			mMapTextures;
	std::unordered_map<UINT, FTTileMap*>			mMapTileMaps;
	std::unordered_map<UINT, FTPremade*>			mMapPremades;
	std::unordered_map<UINT, FTSpriteAnimation*>	mMapSpriteAnimation;

	std::unordered_map<UINT, std::vector<MeshData>>	mMapMeshes;
	std::unordered_map<UINT, MeshData>				mMapPrimitives;

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
		mItemKey += resCount;
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
		FTRESOURCE* resource = DBG_NEW FTRESOURCE;
		UINT mItemKey = resource->LoadProperties(ifs);
		
		if(KeyExists(mItemKey, resMap))
		{
			FTRESOURCE* deprecated = resMap.at(mItemKey);
			delete deprecated;
			resMap.at(mItemKey) = nullptr;
			resMap.erase(mItemKey);
		}
		resMap.insert(std::make_pair(mItemKey, resource));
	}

/// <Creating New Resources> -------------------------------------
/// Template member functions for creating new resources & adding to resource map.
/// </Creating New Resources>
public:
	template<typename FTRESOURCE>
	FTRESOURCE* LoadResource(std::string& filePath, std::unordered_map<UINT, FTRESOURCE*>& resMap)
	{
		std::string fileName = filePath.substr(filePath.rfind("\\") + 1);
		UINT pending = mItemKey + 1;
		if (!ResourceExists<FTRESOURCE*>(pending, filePath, resMap)) {
			printf("Message: Loading FTTexture %s to mItemKey %d. \n", filePath.c_str(), pending);
			FTRESOURCE* res = DBG_NEW FTRESOURCE;
			res->SetFileName(fileName);
			res->SetRelativePath(filePath);
			resMap.insert(std::make_pair(pending, res));
			mItemKey = pending;
			return res;
		}
		else {
			printf("Warning : Resource %s is already loaded to mItemKey %d.\n", filePath.c_str(), pending);
			return nullptr;
		}
	}

// Add newly created resource from components (e.g FTSpriteAnimation)
	template<typename FTRESOURCE>
	void LoadResource(FTRESOURCE* res, std::unordered_map<UINT, FTRESOURCE*>& resMap)
	{
		++mItemKey;
		resMap.insert(std::make_pair(mItemKey, res));
	}

/// <Removing Resources> -------------------------------------
/// Template member functions for removing the loaded resources.
/// </Removing Resources>
public:
	template<typename FTRESOURCE>
	void ClearMap(std::unordered_map<UINT, FTRESOURCE*>& resMap) 
	{
		auto iter = resMap.begin();
		while(iter != resMap.end())
		{
			if ((*iter).second != nullptr)
			{
				delete (*iter).second;
				(*iter).second = nullptr;
				iter = resMap.erase(iter);
			}
			else
				++iter;
		}
		resMap.clear();
	}

	template<typename FTRESOURCE>
	void RemoveResource(UINT mItemKey, std::unordered_map<UINT, FTRESOURCE*>& resMap)
	{
		FTRESOURCE* resource = resMap.at(mItemKey);
		if (resource)
		{
			delete resource;
			resource = nullptr;
			resMap.erase(mItemKey);
		}
		else
			printf("ERROR: ResourceManager::RemoveResource()->mItemKey %d does not exist", mItemKey);
	}

/// <Processing Resources> -------------------------------------
/// Template member functions for processing newly loaded resources. 
/// </Processing Resources>
private:
	void ProcessTexture(FTTexture* texture);
	void ProcessTileMap(FTTileMap* tileMap);
	void ProcessSpriteAnim(FTSpriteAnimation* spriteAnim);
	void ProcessTextures();
	void ProcessPremades();
	void ProcessTileMaps();
	void ProcessSpriteAnims();

/// <Validating Resources> -------------------------------------
/// Template member functions for validating the keys & resources.
/// </Validating Resources>
private:
	template<typename FTRESOURCE>
	bool KeyExists(const UINT mItemKey, const std::unordered_map<UINT, FTRESOURCE>& resMap) {
		if (resMap.find(mItemKey) != resMap.end()) {
			printf("Error: ResourceManager::ResourceExists() -> Resource with mItemKey %d exists\n", mItemKey);
			return true;
		}
		return false;
	}

	template<typename FTRESOURCE>
	bool ResourceExists(const UINT mItemKey, const std::string path, const std::unordered_map<UINT, FTRESOURCE>& resMap) {
		if (0 < resMap.size()) {
			if (!KeyExists(mItemKey, resMap)) {
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