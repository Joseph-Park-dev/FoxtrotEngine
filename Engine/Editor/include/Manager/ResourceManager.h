#pragma once
#include "Manager/ResourceManagerBase.h"

#include "Utility/SingletonMacro.h"
#include "FTDS/Dynamic/DynamicArray.h"
#include "FTDS/Static/FTString.h"
#include "EditorRenderer.h"

#include <ResourceSystem/SupportedResources.h>

//namespace Editor
//{
//	class ResourceManager :
//		public Core::ResourceManagerBase
//	{
//		SINGLETON(ResourceManager)
//
//	public:
//		void SaveResourcesToChunk(std::ofstream& ofs);
//
//		// On Editor, loading resource from .chunk is not necessary, thus skip the process.
//		void PassLoadResourceInChunk(std::ifstream& ifs);
//
//	public:
//		void UpdateUI();
//
//	public:
//		void SetRenderer(Editor::EditorRenderer* renderer);
//
//	private:
//		Core::ResourceManager*	mCoreRes;
//		D3D11::ResourceManager* mD3D11Res;
//
//	private:
//		size_t GetCoreTypeIdx(Common::FTDS::String& fileName);
//		size_t GetD3D11TypeIdx(Common::FTDS::String& fileName);
//
//	public:
//		/// @brief Creates new resources & adding to resource map.
//		/// @tparam FTRESOURCE Non-graphics resource type.
//		template <typename FTRESOURCE>
//		FTRESOURCE* LoadResource(Common::ResourceManagerBase* managerInst, Common::FTDS::String& filePath)
//		{
//			// Get Relative path to Assets folder
//			Common::FTDS::String fileName = ExtractFileName(filePath.C_Str());
//			printf("Message: Loading FTResource %s to key %s. \n", filePath.C_Str(), fileName.C_Str());
//
//			FTResourceDef resDef{ fileName, filePath };
//			FTRESOURCE* res = DBG_NEW FTRESOURCE(resDef);
//
//			// ResourceManager::GetInstance()->AbsoluteToRelativePath(res);
//
//			Common::FTDS::HashMap<FTRESOURCE*> resMap = managerInst->GetResMap<FTRESOURCE>();
//			if (resMap.IsFull())
//				resMap.Reserve(resMap.GetSize() + 5);
//
//			if (res)
//				resMap.Insert(fileName, res);
//			else
//			{
//				delete res;
//				res = nullptr;
//			}
//			return res;
//		}
//
//		/// @brief Creates new resources & adding to resource map.
//		/// @tparam FTRESOURCE Graphics resource type.
//		/// @param renderer Graphics resource requires renderer instance for initialization.
//		template <typename FTRESOURCE>
//		FTRESOURCE* LoadResource(Common::FTDS::String& filePath, Editor::EditorRenderer* renderer)
//		{
//			assert(renderer);
//
//			// Get Relative path to Assets folder
//			Common::FTDS::String fileName = ExtractFileName(filePath.C_Str());
//			printf("Message: Loading FTResource %s to key %s. \n", filePath.C_Str(), fileName.C_Str());
//
//			FTResourceDef resDef{ fileName, filePath };
//			FTRESOURCE* res = DBG_NEW FTRESOURCE(resDef, renderer);
//			return res;
//		}
//
//		// This is used to avoid additional resource loading in PassLoadResourceInChunk(ifs)
//		template <typename FTRESOURCE>
//		void LoadDummyResource(std::ifstream& ifs, ResArray* entireRes, size_t& resCount)
//		{
//			if (resCount < 1)
//				return;
//
//			Common::FTDS::HashMap<FTResource*>& resMap = entireRes->At(FTRESOURCE::Type);
//			resMap.Reserve(resCount);
//			while (0 < resCount)
//			{
//				Common::FTDS::String dummyStr;
//				FileIOHelper::BeginDataPackLoad(ifs);
//				FileIOHelper::LoadBasicString(ifs, dummyStr);
//				FileIOHelper::LoadBasicString(ifs, dummyStr);
//
//				assert(0 < resMap->Capacity());
//				// resMap->Insert(res->FileName(), res);
//				--resCount; // Key of the next resource to be imported.
//			}
//		}
//
//		// Add newly created resource from components (e.g FTSpriteAnimation)
//		template <typename FTRESOURCE>
//		void LoadResource(FTRESOURCE* res)
//		{
//			GetResMap<FTRESOURCE>()->Insert(FTRESOURCE::Type, res);
//		}
//
//		////////////////////////
//		// Removing resources //
//		////////////////////////
//	private:
//		// template <typename FTRESOURCE>
//		// void ClearMap(std::unordered_map<Common::FTDS::String, FTRESOURCE*>& resMap)
//		//{
//		//	auto iter = resMap.begin();
//		//	while (iter != resMap.end())
//		//	{
//		//		if ((*iter).second)
//		//		{
//		//			delete (*iter).second;	// De-allocate value
//		//			(*iter).second = nullptr;
//		//		}
//		//		++iter;
//		//	}
//		//	resMap.clear();
//		// }
//
//		template <typename FTRESOURCE>
//		void RemoveResource(Common::FTDS::String key, Common::FTDS::HashMap<FTRESOURCE*>* resMap)
//		{
//			resMap->Erase(key);
//			// printf("ERROR: ResourceManager::RemoveResource()->key %s does not exist", key);
//		}
//
//		//////////////////////////
//		// Validating Resources //
//		//////////////////////////
//	private:
//		template <typename FTRESOURCE>
//		bool KeyExists(Common::FTDS::String key, Common::FTDS::HashMap<FTRESOURCE>* resMap)
//		{
//			Common::FTDS::Record<FTRESOURCE>* res = nullptr;
//			res									= resMap->At(key);
//			return res;
//		}
//	};
//} // namespace Editor