#pragma once
#include "Manager/ResourceManagerBase.h"

#include "SingletonMacro.h"
#include "FTDS/Dynamic/DynamicArray.h"
#include "FTDS/Static/FTString.h"
#include "EditorRenderer.h"

#include <./Core/src/Manager/ResourceManager.h>
#include <./D3D11/src/Manager/ResourceManager.h>

#include <ResourceSystem/SupportedResources.h>

namespace Editor
{
	class ResourceManager :
		public Core::ResourceManagerBase
	{
		SINGLETON(ResourceManager)

	public:
		void LoadDefaultResources() override;
		void LoadAllResourcesInAsset();

	public:
		void SaveResources(std::ofstream& ofs);

		/// @brief Load a resource with type extracted from filePath.
		/// @param aborted File paths whose loading was aborted.
		/// After the first LoadResource() these will be attempted to be loaded again.
		void LoadCoreResByType(const char* filePath, Core::FTDS::DynamicArray<Core::FTDS::String*>& aborted);
		void LoadD3D11ResByType(const char* filePath, Core::FTDS::DynamicArray<Core::FTDS::String*>& aborted);

		// On Editor, loading resource from .chunk is not necessary, thus skip the process.
		void PassLoadResourceInChunk(std::ifstream& ifs);

	public:
		template <typename FTRESOURCE>
		void SaveResourceToChunk(Core::ResourceManagerBase* managerInst, std::ofstream& ofs)
		{
			Core::ResArray* resArr = managerInst->GetResArray();
			for (auto mapIter = resArr->Begin(); mapIter != resArr->End(); ++mapIter)
			{
				for (auto res = (*mapIter).Begin(); res != (*mapIter).End(); ++res)
				{
					Core::FileIOHelper::SaveString(
						ofs,
						Core::ChunkKey::FTResource::FILE_NAME,
						managerInst->GetResName(FTRESOURCE::Type, (*res)));
				}
			}
		}

	public:
		void UpdateUI();

	public:
		void SetRenderer(Editor::EditorRenderer* renderer);

	protected:
		void RegisterMemberFuncs() override;

	private:
		Core::GET_RES_MANAGER_INST	mGetCoreResManagerFunc;
		D3D11::GET_RES_MANAGER_INST mGetD3D11ResManagerFunc;
		Editor::EditorRenderer*		mRenderer;

	private:
		size_t GetCoreTypeIdx(Core::FTDS::String& fileName);
		size_t GetD3D11TypeIdx(Core::FTDS::String& fileName);

	public:
		/// @brief Creates new resources & adding to resource map.
		/// @tparam FTRESOURCE Non-graphics resource type.
		template <typename FTRESOURCE>
		FTRESOURCE* LoadResource(Core::ResourceManagerBase* managerInst, Core::FTDS::String& filePath)
		{
			// Get Relative path to Assets folder
			Core::FTDS::String fileName = ExtractFileName(filePath.C_Str());
			printf("Message: Loading FTResource %s to key %s. \n", filePath.C_Str(), fileName.C_Str());

			FTResourceDef resDef{ fileName, filePath };
			FTRESOURCE* res = DBG_NEW FTRESOURCE(resDef);

			// ResourceManager::GetInstance()->AbsoluteToRelativePath(res);

			FTDS::HashMap<FTRESOURCE*> resMap = managerInst->GetResMap<FTRESOURCE>();
			if (resMap.IsFull())
				resMap.Reserve(resMap.GetSize() + 5);

			if (res)
				resMap.Insert(fileName, res);
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
		FTRESOURCE* LoadResource(Core::FTDS::String& filePath, Editor::EditorRenderer* renderer)
		{
			assert(renderer);

			// Get Relative path to Assets folder
			Core::FTDS::String fileName = ExtractFileName(filePath.C_Str());
			printf("Message: Loading FTResource %s to key %s. \n", filePath.C_Str(), fileName.C_Str());

			FTResourceDef resDef{ fileName, filePath };
			FTRESOURCE* res = DBG_NEW FTRESOURCE(resDef, renderer);
			return res;
		}

		// This is used to avoid additional resource loading in PassLoadResourceInChunk(ifs)
		template <typename FTRESOURCE>
		void LoadDummyResource(std::ifstream& ifs, ResArray* entireRes, size_t& resCount)
		{
			if (resCount < 1)
				return;

			FTDS::HashMap<FTResource*>& resMap = entireRes->At(FTRESOURCE::Type);
			resMap.Reserve(resCount);
			while (0 < resCount)
			{
				Core::FTDS::String dummyStr;
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
		void LoadResource(FTRESOURCE* res)
		{
			GetResMap<FTRESOURCE>()->Insert(FTRESOURCE::Type, res);
		}

		////////////////////////
		// Removing resources //
		////////////////////////
	private:
		// template <typename FTRESOURCE>
		// void ClearMap(std::unordered_map<Core::FTDS::String, FTRESOURCE*>& resMap)
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
		void RemoveResource(Core::FTDS::String key, Core::FTDS::HashMap<FTRESOURCE*>* resMap)
		{
			resMap->Erase(key);
			// printf("ERROR: ResourceManager::RemoveResource()->key %s does not exist", key);
		}

		//////////////////////////
		// Validating Resources //
		//////////////////////////
	private:
		template <typename FTRESOURCE>
		bool KeyExists(Core::FTDS::String key, Core::FTDS::HashMap<FTRESOURCE>* resMap)
		{
			Core::FTDS::Record<FTRESOURCE>* res = nullptr;
			res									= resMap->At(key);
			return res;
		}

		template <typename FTRESOURCE>
		void DisplayLoadedResources(const char* label, Core::FTDS::HashMap<FTRESOURCE*>& resMap)
		{
			if (ImGui::TreeNode(label))
			{
				for (auto iter = resMap.Begin(); iter != resMap.End(); ++iter)
				{
					if (ImGui::TreeNode((*iter)->Key().C_Str()))
					{
						(*iter)->Value()->UpdateUI();
						ImGui::TreePop();
					}
				}
				ImGui::TreePop();
			}
		}
	};
} // namespace Editor