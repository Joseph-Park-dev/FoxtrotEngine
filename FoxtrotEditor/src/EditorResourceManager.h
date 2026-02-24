#pragma once
#include "Managers/ResourceManager.h"

#include "Utils/StrAssign.h"
#include "ResourceSystem/GenericData/FTText.h"
#include "ResourceSystem/GenericData/FTJSON.h"
#include "ResourceSystem/FTTileMap.h"
#include "ResourceSystem/Animation/FTSpineAnimation.h"
#include "ResourceSystem/Animation/FTSpriteAnimation.h"
#include "ResourceSystem/FTPremade.h"
#include "ResourceSystem/FTFont/FTFont.h"
#include "ResourceSystem/D3D11PSO.h"

#include <FTCoreEditor.h>

class EditorResourceManager :
	public ResourceManager
{
	SINGLETON(EditorResourceManager)

public:
	void Initialize(FoxtrotRenderer* renderer) override;

public:
	void SaveResources(std::ofstream& ofs);

	void LoadAllResourcesInAsset();

	/// @brief Load a resource with type extracted from filePath.
	/// @param aborted File paths whose loading was aborted.
	/// After the first LoadResource() these will be attempted to be loaded again.
	void LoadResByType(const char* filePath, FTDS::DynamicArray<FTDS::String*>& aborted);

	// On Editor, loading resource from .chunk is not necessary, thus skip the process.
	void PassLoadResourceInChunk(std::ifstream& ifs);

public:
	FTDS::HashMap<D3D11PSO*>*		   GetPSOs() override;
	FTDS::HashMap<FTSprite*>*		   GetSprites() override;
	FTDS::HashMap<FTTileMap*>*		   GetTileMaps() override;
	FTDS::HashMap<FTPremade*>*		   GetPremades() override;
	FTDS::HashMap<FTMaterial*>*		   GetMaterials() override;
	FTDS::HashMap<FTMeshGroup*>*	   GetMeshGroups() override;
	FTDS::HashMap<FTSpriteAnimation*>* GetSpriteAnimations() override;
	FTDS::HashMap<FTSpineAnimation*>*  GetSpineAnimations() override;
	FTDS::HashMap<Sound*>*			   GetSounds() override;
	FTDS::HashMap<FTCSV*>*			   GetCSVs() override;
	FTDS::HashMap<FTJSON*>*			   GetJSONs() override;
	FTDS::HashMap<FTText*>*			   GetTexts() override;
	FTDS::HashMap<FTFont*>*			   GetFonts() override;

public:
	template <typename FTRESOURCE>
	void SaveResourceToChunk(std::ofstream& ofs, FTDS::HashMap<FTRESOURCE*>* resArr)
	{
		resArr->IterateAllValues([&](FTRESOURCE* res) {
			if (res->IsReferenced())
			{
				FTDS::String path = res->GetRelativePath();
				ResourceManager::GetInstance()->AbsoluteToRelativePath(path);

				// ResourceManager::GetInstance()->AbsoluteToRelativePath(res);
				FileIOHelper::BeginDataPackSave(ofs, res->GetFileName());
				FileIOHelper::SaveString(ofs, ChunkKey::FTResource::FILE_NAME, res->GetFileName());
				FileIOHelper::SaveString(ofs, ChunkKey::FTResource::RELATIVE_PATH, path.C_Str());
				FileIOHelper::EndDataPackSave(ofs, res->GetFileName());
			}
		});
	}

public:
	void UpdateUI();

private:
	ResType GetResType(FTDS::String& fileName);

public:
	/// @brief Creates new resources & adding to resource map.
	/// @tparam FTRESOURCE Non-graphics resource type.
	template <typename FTRESOURCE>
	FTRESOURCE* LoadResource(FTDS::String& filePath, FTDS::HashMap<FTRESOURCE*>* resMap)
	{
		// Get Relative path to Assets folder
		FTDS::String fileName = ExtractFileName(filePath.C_Str());
		printf("Message: Loading FTResource %s to key %s. \n", filePath.C_Str(), fileName.C_Str());

		FTResourceDef resDef{ fileName, filePath };
		FTRESOURCE* res = DBG_NEW FTRESOURCE(resDef);

		// ResourceManager::GetInstance()->AbsoluteToRelativePath(res);

		if (resMap->IsFull())
			resMap->Reserve(resMap->GetSize() + 5);

		if (res->IsProcessed())
			resMap->Insert(fileName, res);
		else
		{
			delete res;
			res = nullptr;
		}
		return res;
	}

	/// @brief Creates new resources & adding to resource map.
	/// @tparam FTRESOURCE Graphics resource type.
	/// @param renderer Graphics resource requires renderer instance for initialization.
	template <typename FTRESOURCE>
	FTRESOURCE* LoadResource(FTDS::String& filePath, FTDS::HashMap<FTRESOURCE*>* resMap, FoxtrotRenderer* renderer)
	{
		assert(renderer);

		// Get Relative path to Assets folder
		FTDS::String fileName = ExtractFileName(filePath.C_Str());
		printf("Message: Loading FTResource %s to key %s. \n", filePath.C_Str(), fileName.C_Str());

		FTResourceDef resDef{ fileName, filePath };
		FTRESOURCE* res = DBG_NEW FTRESOURCE(resDef, renderer);

		if (resMap->IsFull())
			resMap->Reserve(resMap->GetSize() + 5);

		if (res->IsProcessed())
			resMap->Insert(fileName, res);
		else
		{
			delete res;
			res = nullptr;
		}
		return res;
	}

	// This is used to avoid additional resource loading in PassLoadResourceInChunk(ifs)
	template <typename FTRESOURCE>
	void LoadDummyResource(std::ifstream& ifs, FTDS::HashMap<FTRESOURCE*>* resMap, size_t& resCount)
	{
		if (resCount < 1)
			return;

		resMap->Reserve(resCount);
		while (0 < resCount)
		{
			FTDS::String dummyStr;
			FileIOHelper::BeginDataPackLoad(ifs);
			FileIOHelper::LoadBasicString(ifs, dummyStr);
			FileIOHelper::LoadBasicString(ifs, dummyStr);

			assert(0 < resMap->Capacity());
			// resMap->Insert(res->FileName(), res);
			--resCount; // Key of the next resource to be imported.
		}
	}

	// Add newly created resource from components (e.g FTSpriteAnimation)
	template <typename FTRESOURCE>
	void LoadResource(FTRESOURCE* res, FTDS::HashMap<FTRESOURCE*>* resMap)
	{
		resMap->Insert(res->FileName(), res);
	}

	////////////////////////
	// Removing resources //
	////////////////////////
private:
	// template <typename FTRESOURCE>
	// void ClearMap(std::unordered_map<FTDS::String, FTRESOURCE*>& resMap)
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
	// }

	template <typename FTRESOURCE>
	void RemoveResource(FTDS::String key, FTDS::HashMap<FTRESOURCE*>* resMap)
	{
		resMap->Erase(key);
		// printf("ERROR: ResourceManager::RemoveResource()->key %s does not exist", key);
	}

	//////////////////////////
	// Validating Resources //
	//////////////////////////
private:
	template <typename FTRESOURCE>
	bool KeyExists(FTDS::String key, FTDS::HashMap<FTRESOURCE>* resMap)
	{
		FTDS::Record<FTRESOURCE>* res = nullptr;
		res							  = resMap->At(key);
		return res;
	}

	template <typename FTRESOURCE>
	void DisplayLoadedResources(const char* label, FTDS::HashMap<FTRESOURCE*>* resMap)
	{
		if (ImGui::TreeNode(label))
		{
			resMap->IterateAllValues(
				[&](FTRESOURCE* res) {
					if (ImGui::TreeNode(res->GetFileName().C_Str()))
					{
						res->UpdateUI();
						ImGui::TreePop();
					}
				});
			ImGui::TreePop();
		}
	}
};