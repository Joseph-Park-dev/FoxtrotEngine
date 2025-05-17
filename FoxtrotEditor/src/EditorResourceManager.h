#pragma once
#include "Managers/ResourceManager.h"

#include "Utils/StrAssign.h"

class EditorResourceManager : public ResourceManager
{
	SINGLETON(EditorResourceManager)

public:
	void LoadAllResourcesInAsset();
	void LoadResByType(FTDS::String& fileName);
	void LoadMaterials() override;

	void PassLoadResourceInChunk(std::ifstream& ifs);

//public:
//	std::unordered_map<FTDS::String, FTTexture*>& GetTexturesMap();
//	// I know the name feels so funny...
//	std::unordered_map<FTDS::String, FTTileMap*>&		 GetTileMapsMap();
//	std::unordered_map<FTDS::String, FTSpriteSheet*>&	 GetSpriteSheetsMap();
//	std::unordered_map<FTDS::String, FTSpriteAnimation*>& GetSpriteAnimMap();
//	std::unordered_map<FTDS::String, FTBasicMeshGroup*>&	 GetMeshGroupsMap();
//
//	std::unordered_map<FTDS::String, FTVertexShader*>& GetVertexShadersMap();
//	std::unordered_map<FTDS::String, FTPixelShader*>&  GetPixelShadersMap();
//
//	std::unordered_map<FTDS::String, FTMaterial*>& GetMapMaterials();
//
//	std::unordered_map<FTDS::String, FTCSV*>&  GetMapCSVs();
//	std::unordered_map<FTDS::String, FTJSON*>& GetMapJSONs();
//
//public:
//	FTTexture*		   GetLoadedTexture(FTDS::String key) override;
//	FTTileMap*		   GetLoadedTileMap(FTDS::String key) override;
//	FTSpriteSheet*	   GetLoadedSpriteSheet(FTDS::String key) override;
//	FTPremade*		   GetLoadedPremade(FTDS::String key) override;
//	FTPixelShader*	   GetLoadedPixelShader(FTDS::String key) override;
//	FTMaterial*		   GetLoadedMaterial(FTDS::String key) override;
//	FTBasicMeshGroup*  GetLoadedMesh(FTDS::String key) override;
//	FTSpriteAnimation* GetLoadedSpriteAnim(FTDS::String key) override;
//	FTCSV*			   GetLoadedCSV(FTDS::String key) override;
//	FTJSON*			   GetLoadedJSON(FTDS::String key) override;

public:
	void Initialize(FoxtrotRenderer* renderer) override;
	//void DeleteAll() override;

//private:
//	void ProcessTextures() override;
//	void ProcessMeshGroups() override;
//	void ProcessPremades() override;
//	void ProcessTileMaps() override;
//	void ProcessSpriteSheets() override;
//	void ProcessSpriteAnims() override;
//	void ProcessCSVs() override;
//	void ProcessJSONs() override;
//
//	void ProcessMaterials() override;
//	void ProcessVertexShaders() override;
//	void ProcessPixelShaders() override;

public:
	void UpdateUI();

	//////////////////////
	// Foxtrot resources//
	//////////////////////
//private:
//	std::unordered_map<FTDS::String, FTTexture*>			mMapTextures;
//	std::unordered_map<FTDS::String, FTTileMap*>			mMapTileMaps;
//	std::unordered_map<FTDS::String, FTSpriteSheet*>		mMapSpriteSheets;
//	std::unordered_map<FTDS::String, FTPremade*>			mMapPremades;
//	std::unordered_map<FTDS::String, FTSpriteAnimation*> mMapSpriteAnimation;
//
//	// A mesh group usually represents a 3D model.
//	std::unordered_map<FTDS::String, FTBasicMeshGroup*> mMapMeshGroups;
//
//	std::unordered_map<FTDS::String, FTVertexShader*> mMapVertexShaders;
//	std::unordered_map<FTDS::String, FTPixelShader*>	 mMapPixelShaders;
//
//	std::unordered_map<FTDS::String, FTMaterial*> mMapMaterials;
//
//	////////////////////////////
//	// Generic-type resources //
//	////////////////////////////
//	std::unordered_map<FTDS::String, FTCSV*>	 mMapCSVs;
//	std::unordered_map<FTDS::String, FTJSON*> mMapJSONs;

private:
	ResType GetResType(FTDS::String& fileName);

public:
	/// Template member functions for creating new resources & adding to resource map.
	template <typename FTRESOURCE>
	FTRESOURCE* LoadResource(FTDS::String& filePath, FTDS::HashChainMap<FTRESOURCE*>* resMap)
	{
		// Get Relative path to Assets folder
		FTDS::String fileName = filePath.SubStr(filePath.RFind("\\") + 1);

		if (!KeyExists<FTRESOURCE*>(fileName.C_Str(), resMap))
		{
			printf("Message: Loading FTResource %s to key %s. \n", filePath.C_Str(), fileName.C_Str());

			FTRESOURCE* res = DBG_NEW FTRESOURCE;
			res->SetFileName(fileName);
			res->SetRelativePath(filePath);

			resMap->Insert(fileName);
			return res;
		}
		else
		{
			printf("Warning : Resource %s is already loaded to key %s.\n", filePath.C_Str(), fileName.C_Str());
			return nullptr;
		}
	}

	// Add newly created resource from components (e.g FTSpriteAnimation)
	template <typename FTRESOURCE>
	void LoadResource(FTRESOURCE* res, FTDS::HashChainMap<FTRESOURCE*>* resMap)
	{
		resMap.insert(std::make_pair(res->GetFileName().c_str(), res));
	}

	////////////////////////
	// Removing resources //
	////////////////////////
private:
	//template <typename FTRESOURCE>
	//void ClearMap(std::unordered_map<FTDS::String, FTRESOURCE*>& resMap)
	//{
	//	auto iter = resMap.begin();
	//	while (iter != resMap.end())
	//	{
	//		if ((*iter).second)
	//		{
	//			delete (*iter).second;	// De-allocate value
	//			(*iter).second = nullptr;
	//		}
	//		++iter;
	//	}
	//	resMap.clear();
	//}

	template <typename FTRESOURCE>
	void RemoveResource(FTDS::String key, FTDS::HashChainMap<FTRESOURCE*>* resMap)
	{
		resMap->Erase(key);
		//printf("ERROR: ResourceManager::RemoveResource()->key %s does not exist", key);
	}

	//////////////////////////
	// Validating Resources //
	//////////////////////////
private:
	template <typename FTRESOURCE>
	bool KeyExists(FTDS::String key, FTDS::HashChainMap<FTRESOURCE*>* resMap)
	{
		if (resMap->At(key));
		{
			printf("Error: ResourceManager::ResourceExists() -> Resource with key %s exists\n", key);
			return true;
		}
		return false;
	}

	/*template <typename FTRESOURCE>
	bool ResourceExists(const FTDS::String key, const FTDS::String path, const std::unordered_map<FTDS::String, FTRESOURCE>& resMap)
	{
		if (0 < resMap.size())
		{
			if (!KeyExists(key, resMap))
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
	}*/
};