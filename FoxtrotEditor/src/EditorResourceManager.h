#pragma once
#include "Managers/ResourceManager.h"

#include "Utils/StrAssign.h"
#include "ResourceSystem/GenericData/FTText.h"
#include "ResourceSystem/GenericData/FTJSON.h"
#include "ResourceSystem/Animation/FTSpineAnimation.h"

class EditorResourceManager : public ResourceManager
{
	SINGLETON(EditorResourceManager)

public:
	void SaveResources(std::ofstream& ofs);

	void LoadAllResourcesInAsset();
	void LoadResByType(const char* fileName);
	void LoadMaterials() override;

	// On Editor, loading resource from .chunk is not necessary, thus skip the process.
	void PassLoadResourceInChunk(std::ifstream& ifs);

public:
	template <typename FTRESOURCE>
	void SaveResourceToChunk(std::ofstream& ofs, FTDS::HashChainMap<FTRESOURCE*>* resArr)
	{
		resArr->IterateAllValues([&](FTRESOURCE* res) {
			if (res->IsReferenced())
			{
				AbsoluteToRelativePath(res);
				FileIOHelper::BeginDataPackSave(ofs, res->FileName());
				FileIOHelper::SaveString(ofs, ChunkKey::FILE_NAME, res->FileName());
				FileIOHelper::SaveString(ofs, ChunkKey::RELATIVE_PATH, res->RelativePath().C_Str());
				FileIOHelper::EndDataPackSave(ofs, res->FileName());
			}
		});
	}

public:
	void UpdateUI();

private:
	ResType GetResType(FTDS::String& fileName);

public:
	/// Template member functions for creating new resources & adding to resource map.
	template <typename FTRESOURCE>
	FTRESOURCE* LoadResource(FTDS::String& filePath, FTDS::HashChainMap<FTRESOURCE*>* resMap)
	{
		// Get Relative path to Assets folder
		FTDS::String fileName = ExtractFileName(filePath.C_Str());

		if (!KeyExists<FTRESOURCE*>(fileName, resMap))
		{
			printf("Message: Loading FTResource %s to key %s. \n", filePath.C_Str(), fileName.C_Str());

			FTRESOURCE* res = DBG_NEW FTRESOURCE;
			res->SetFileName(fileName);

			res->SetRelativePath(filePath);
			AbsoluteToRelativePath(res);

			resMap->Insert(fileName, res);
			return res;
		}
		else
		{
			printf("Warning : Resource %s is already loaded to key %s.\n", filePath.C_Str(), fileName.C_Str());
			return nullptr;
		}
	}

	// This is used to avoid additional resource loading in PassLoadResourceInChunk(ifs)
	template <typename FTRESOURCE>
	void LoadDummyResource(std::ifstream& ifs, FTDS::HashChainMap<FTRESOURCE*>* resMap, size_t& resCount)
	{
		if (resCount < 1)
			return;

		resMap->Reserve(resCount);
		while (0 < resCount)
		{
			FTRESOURCE* res = DBG_NEW FTRESOURCE;
			FileIOHelper::BeginDataPackLoad(ifs);
			FileIOHelper::LoadBasicString(ifs, res->RelativePath());
			FileIOHelper::LoadBasicString(ifs, res->FileName());

			assert(0 < resMap->Capacity());
			resMap->Insert(res->FileName(), res);
			delete res;
			--resCount; // Key of the next resource to be imported.
		}
	}

	// Add newly created resource from components (e.g FTSpriteAnimation)
	template <typename FTRESOURCE>
	void LoadResource(FTRESOURCE* res, FTDS::HashChainMap<FTRESOURCE*>* resMap)
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
	void RemoveResource(FTDS::String key, FTDS::HashChainMap<FTRESOURCE*>* resMap)
	{
		resMap->Erase(key);
		// printf("ERROR: ResourceManager::RemoveResource()->key %s does not exist", key);
	}

	//////////////////////////
	// Validating Resources //
	//////////////////////////
private:
	template <typename FTRESOURCE>
	bool KeyExists(FTDS::String key, FTDS::HashChainMap<FTRESOURCE>* resMap)
	{
		FTDS::RecordNode<FTRESOURCE>* res = nullptr;
		res								  = resMap->At(key);
		return res;
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