#pragma once
#include "Managers/ResourceManager.h"

#include "Utils/StrAssign.h"

class EditorResourceManager : public ResourceManager
{
	SINGLETON(EditorResourceManager)

public:
	void LoadAllResourcesInAsset();
	void LoadResByType(std::string& fileName);
	void LoadMaterials() override;

	void PassLoadResourceInChunk(std::ifstream& ifs);

public:
	std::unordered_map<const char*, FTTexture*>& GetTexturesMap();
	// I know the name feels so funny...
	std::unordered_map<const char*, FTTileMap*>&		 GetTileMapsMap();
	std::unordered_map<const char*, FTSpriteSheet*>&	 GetSpriteSheetsMap();
	std::unordered_map<const char*, FTSpriteAnimation*>& GetSpriteAnimMap();
	std::unordered_map<const char*, FTBasicMeshGroup*>&	 GetMeshGroupsMap();

	std::unordered_map<const char*, FTVertexShader*>& GetVertexShadersMap();
	std::unordered_map<const char*, FTPixelShader*>&  GetPixelShadersMap();

	std::unordered_map<const char*, FTMaterial*>& GetMapMaterials();

	std::unordered_map<const char*, FTCSV*>&  GetMapCSVs();
	std::unordered_map<const char*, FTJSON*>& GetMapJSONs();

public:
	FTTexture*		   GetLoadedTexture(const char* key) override;
	FTTileMap*		   GetLoadedTileMap(const char* key) override;
	FTSpriteSheet*	   GetLoadedSpriteSheet(const char* key) override;
	FTPremade*		   GetLoadedPremade(const char* key) override;
	FTPixelShader*	   GetLoadedPixelShader(const char* key) override;
	FTMaterial*		   GetLoadedMaterial(const char* key) override;
	FTBasicMeshGroup*  GetLoadedMesh(const char* key) override;
	FTSpriteAnimation* GetLoadedSpriteAnim(const char* key) override;
	FTCSV*			   GetLoadedCSV(const char* key) override;
	FTJSON*			   GetLoadedJSON(const char* key) override;

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
	std::unordered_map<const char*, FTTexture*>			mMapTextures;
	std::unordered_map<const char*, FTTileMap*>			mMapTileMaps;
	std::unordered_map<const char*, FTSpriteSheet*>		mMapSpriteSheets;
	std::unordered_map<const char*, FTPremade*>			mMapPremades;
	std::unordered_map<const char*, FTSpriteAnimation*> mMapSpriteAnimation;

	// A mesh group usually represents a 3D model.
	std::unordered_map<const char*, FTBasicMeshGroup*> mMapMeshGroups;

	std::unordered_map<const char*, FTVertexShader*> mMapVertexShaders;
	std::unordered_map<const char*, FTPixelShader*>	 mMapPixelShaders;

	std::unordered_map<const char*, FTMaterial*> mMapMaterials;

	////////////////////////////
	// Generic-type resources //
	////////////////////////////
	std::unordered_map<const char*, FTCSV*>	 mMapCSVs;
	std::unordered_map<const char*, FTJSON*> mMapJSONs;

private:
	ResType GetResType(std::string& fileName);

public:
	/// Template member functions for creating new resources & adding to resource map.
	template <typename FTRESOURCE>
	FTRESOURCE* LoadResource(std::string& filePath, std::unordered_map<const char*, FTRESOURCE*>& resMap)
	{
		// Get Relative path to Assets folder
		std::string fileName = filePath.substr(filePath.rfind("\\") + 1);

		if (!ResourceExists<FTRESOURCE*>(fileName.c_str(), filePath, resMap))
		{
			printf("Message: Loading FTResource %s to key %s. \n", filePath.c_str(), fileName.c_str());

			const char* key = FTDS::MakeHeapStr(fileName.c_str());

			FTRESOURCE* res = DBG_NEW FTRESOURCE;
			res->SetFileName(fileName);
			res->SetRelativePath(filePath);

			resMap.insert(std::make_pair(key, res));
			return res;
		}
		else
		{
			printf("Warning : Resource %s is already loaded to key %s.\n", filePath.c_str(), fileName.c_str());
			return nullptr;
		}
	}

	// Add newly created resource from components (e.g FTSpriteAnimation)
	template <typename FTRESOURCE>
	void LoadResource(FTRESOURCE* res, std::unordered_map<const char*, FTRESOURCE*>& resMap)
	{
		resMap.insert(std::make_pair(res->GetFileName().c_str(), res));
	}

	////////////////////////
	// Removing resources //
	////////////////////////
private:
	template <typename FTRESOURCE>
	void ClearMap(std::unordered_map<const char*, FTRESOURCE*>& resMap)
	{
		auto iter = resMap.begin();
		while (iter != resMap.end())
		{
			if ((*iter).second)
			{
				const char* key = (*iter).first;
				delete[] key;

				delete (*iter).second;	// De-allocate value
				(*iter).second = nullptr;
			}
			++iter;
		}
		resMap.clear();
	}

	template <typename FTRESOURCE>
	void RemoveResource(const char* key, std::unordered_map<const char*, FTRESOURCE*>& resMap)
	{
		FTRESOURCE* resource = resMap.at(key);
		if (resource)
		{
			delete resource;
			resource = nullptr;
			resMap.erase(key);
		}
		else
			printf("ERROR: ResourceManager::RemoveResource()->key %s does not exist", key);
	}

	//////////////////////////
	// Validating Resources //
	//////////////////////////
private:
	template <typename FTRESOURCE>
	bool KeyExists(const char* key, const std::unordered_map<const char*, FTRESOURCE>& resMap)
	{
		if (resMap.find(key) != resMap.end())
		{
			printf("Error: ResourceManager::ResourceExists() -> Resource with key %s exists\n", key);
			return true;
		}
		return false;
	}

	template <typename FTRESOURCE>
	bool ResourceExists(const const char* key, const std::string path, const std::unordered_map<const char*, FTRESOURCE>& resMap)
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
	}
};