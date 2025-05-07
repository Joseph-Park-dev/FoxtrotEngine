#pragma once
#include "Managers/ResourceManager.h"

class EditorResourceManager 
	: public ResourceManager
{
	SINGLETON(EditorResourceManager)

public:
	void LoadAllResourcesInAsset();
	void LoadResByType(std::string& fileName);
	void LoadMaterials() override;

	void PassLoadResourceInChunk(std::ifstream& ifs);

public:
	std::unordered_map<UINT, FTTexture*>& GetTexturesMap();
	// I know the name feels so funny...
	std::unordered_map<UINT, FTTileMap*>&		  GetTileMapsMap();
	std::unordered_map<UINT, FTSpriteSheet*>&	  GetSpriteSheetsMap();
	std::unordered_map<UINT, FTSpriteAnimation*>& GetSpriteAnimMap();
	std::unordered_map<UINT, FTBasicMeshGroup*>&  GetMeshGroupsMap();

	std::unordered_map<UINT, FTVertexShader*>& GetVertexShadersMap();
	std::unordered_map<UINT, FTPixelShader*>&  GetPixelShadersMap();

	std::unordered_map<UINT, FTMaterial*>& GetMapMaterials();

	std::unordered_map<UINT, FTCSV*>&  GetMapCSVs();
	std::unordered_map<UINT, FTJSON*>& GetMapJSONs();

public:
	FTTexture*		   GetLoadedTexture(const UINT key) override;
	FTTexture*		   GetLoadedTexture(const char* name) override;
	FTTileMap*		   GetLoadedTileMap(const UINT key) override;
	FTSpriteSheet*	   GetLoadedSpriteSheet(const UINT key) override;
	FTPremade*		   GetLoadedPremade(const UINT key) override;
	FTPremade*		   GetLoadedPremade(const char* fileName) override;
	FTPixelShader*	   GetLoadedPixelShader(const UINT key) override;
	FTMaterial*		   GetLoadedMaterial(const UINT key) override;
	FTBasicMeshGroup*  GetLoadedMesh(const UINT key) override;
	FTSpriteAnimation* GetLoadedSpriteAnim(const UINT key) override;
	FTCSV*			   GetLoadedCSV(const UINT key) override;
	FTJSON*			   GetLoadedJSON(const UINT key) override;

public:
	void Initialize(FoxtrotRenderer* renderer) override;
	void DeleteAll() override;

private:
	void ProcessTextures() override;
	void ProcessMeshGroups() override;
	void ProcessPremades() override;
	void ProcessTileMaps() override;
	void ProcessSpriteSheets() override;
	void ProcessSpriteAnims() override;
	void ProcessCSVs() override;
	void ProcessJSONs() override;

	void ProcessMaterials() override;
	void ProcessVertexShaders() override;
	void ProcessPixelShaders() override;

public:
	void UpdateUI();

	//////////////////////
	// Foxtrot resources//
	//////////////////////
private:
	std::unordered_map<UINT, FTTexture*>		 mMapTextures;
	std::unordered_map<UINT, FTTileMap*>		 mMapTileMaps;
	std::unordered_map<UINT, FTSpriteSheet*>	 mMapSpriteSheets;
	std::unordered_map<UINT, FTPremade*>		 mMapPremades;
	std::unordered_map<UINT, FTSpriteAnimation*> mMapSpriteAnimation;

	// A mesh group usually represents a 3D model.
	std::unordered_map<UINT, FTBasicMeshGroup*> mMapMeshGroups;

	std::unordered_map<UINT, FTVertexShader*> mMapVertexShaders;
	std::unordered_map<UINT, FTPixelShader*>  mMapPixelShaders;

	std::unordered_map<UINT, FTMaterial*> mMapMaterials;

	////////////////////////////
	// Generic-type resources //
	////////////////////////////
	std::unordered_map<UINT, FTCSV*>  mMapCSVs;
	std::unordered_map<UINT, FTJSON*> mMapJSONs;

private:
	ResType GetResType(std::string& fileName);

public:
	/// Template member functions for creating new resources & adding to resource map.
	template <typename FTRESOURCE>
	FTRESOURCE* LoadResource(std::string& filePath, std::unordered_map<UINT, FTRESOURCE*>& resMap)
	{
		// Get Relative path to Assets folder
		std::string fileName = filePath.substr(filePath.rfind("\\") + 1);
		UINT		itemKey	 = resMap.size();

		if (!ResourceExists<FTRESOURCE*>(itemKey, filePath, resMap))
		{
			printf("Message: Loading FTResource %s to mItemKey %d. \n", filePath.c_str(), itemKey);
			FTRESOURCE* res = DBG_NEW FTRESOURCE;
			res->SetFileName(fileName);
			res->SetRelativePath(filePath);
			resMap.insert(std::make_pair(itemKey, res));
			return res;
		}
		else
		{
			printf("Warning : Resource %s is already loaded to mItemKey %d.\n", filePath.c_str(), itemKey);
			return nullptr;
		}
	}

	// Add newly created resource from components (e.g FTSpriteAnimation)
	template <typename FTRESOURCE>
	UINT LoadResource(FTRESOURCE* res, std::unordered_map<UINT, FTRESOURCE*>& resMap)
	{
		UINT key = resMap.size();
		resMap.insert(std::make_pair(key, res));
		return key;
	}

	////////////////////////
	// Removing resources //
	////////////////////////
private:
	template <typename FTRESOURCE>
	void ClearMap(std::unordered_map<UINT, FTRESOURCE*>& resMap)
	{
		auto iter = resMap.begin();
		while (iter != resMap.end())
		{
			if ((*iter).second)
			{
				delete (*iter).second;
				(*iter).second = nullptr;
			}
			++iter;
		}
		resMap.clear();
	}

	template <typename FTRESOURCE>
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

	//////////////////////////
	// Validating Resources //
	//////////////////////////
private:
	template <typename FTRESOURCE>
	bool KeyExists(const UINT mItemKey, const std::unordered_map<UINT, FTRESOURCE>& resMap)
	{
		if (resMap.find(mItemKey) != resMap.end())
		{
			printf("Error: ResourceManager::ResourceExists() -> Resource with mItemKey %d exists\n", mItemKey);
			return true;
		}
		return false;
	}

	template <typename FTRESOURCE>
	bool ResourceExists(const UINT mItemKey, const std::string path, const std::unordered_map<UINT, FTRESOURCE>& resMap)
	{
		if (0 < resMap.size())
		{
			if (!KeyExists(mItemKey, resMap))
			{
				auto iter = resMap.begin();
				for (; iter != resMap.end(); ++iter)
				{
					if ((*iter).second)
					{
						if ((*iter).second->GetRelativePath() == path)
						{
							printf("Error: ResourceManager::ResourceExists() -> Resource with path %s exists\n", path.c_str());
							return true;
						}
					}
				}
				return false;
			}
			return false;
		}
		return false;
	}

	template <typename FTRESOURCE>
	UINT GetMaxKey(const std::unordered_map<UINT, FTRESOURCE>& resMap)
	{
		UINT result = ChunkKey::NullVal::VALUE_NOT_ASSIGNED;
		auto iter = resMap.begin();
		for (; iter != resMap.end(); ++iter)
		{
			if (result < (*iter).first)
				result = (*iter).first;
		}
		return result;
	}
};