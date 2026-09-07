#pragma once
#include <fstream>
#include <type_traits>
#include <stdexcept>
#include "ResourceSystem/FTResource.h"

#include "FTDS/Static/HashMap.h"
#include "FileSystem/FileIOHelper.h"
#include "ResourceSystem/IResource.h"

#ifdef FOXTROT_EDITOR
	#define IMGUI_DEFINE_MATH_OPERATORS
	#include <imgui.h>
#endif // FOXTROT_EDITOR

namespace Common
{
	template <typename FTRESOURCE>
	class ResourcePack
	{
	public:
		/// @brief Load all FTResources described in the .chunk
		/// @tparam FTRESOURCE Type of Resource
		/// @param userData Additional data necessary for resource's constructor.
		/// Pointer to renderer can be a good example for graphics resources
		void LoadResourcesFromChunk(std::ifstream& ifs, void* userData, FTRESOURCE* (*factory)(FTResourceDef&, void*) = nullptr)
		{
			size_t resCount = Common::FileIOHelper::BeginDataPackLoad(ifs).first;
			if (resCount < 1)
				return;

			mResources->Reserve(resCount);
			while (0 < resCount)
			{
				LoadResource(ifs, userData, factory);
				--resCount; // Key of the next resource to be imported.
			}
		}

		void LoadResourcesFromChunk(std::ifstream& ifs)
		{
			size_t resCount = Common::FileIOHelper::BeginDataPackLoad(ifs).first;
			if (resCount < 1)
				return;

			mResources->Reserve(resCount);
			while (0 < resCount)
			{
				LoadResource(ifs);
				--resCount; // Key of the next resource to be imported.
			}
		}

	public:
		Common::FTDS::HashMap<FTRESOURCE*>* GetResMap()
		{
			return mResources;
		}

		FTRESOURCE* GetResource(Common::FTDS::String& key)
		{
			if (key.Equal(Common::ChunkKey::NullVal::NULL_OBJECT))
				return nullptr;

			FTDS::Record<FTRESOURCE*>* rec = mResources->At(key);
			if (!rec)
			{
				Common::Debug::LogError(__LINE__, __FILE__, "Resource is NULL");
				return nullptr;
			}
			return rec->Value();
		}

		FTRESOURCE* GetResource(const char* key)
		{
			if (FTDS::StringEqual(key, Common::ChunkKey::NullVal::NULL_OBJECT))
				return nullptr;

			FTDS::Record<FTRESOURCE*>* rec = mResources->At(key);
			if (!rec)
			{
				Common::Debug::LogError(__LINE__, __FILE__, "Resource is NULL");
				return nullptr;
			}
			return rec->Value();
		}

		void AddResource(const Common::FTDS::String* key, FTRESOURCE* res)
		{
			mResources->Insert(*key, res);
		}

		void AddResource(const char* key, FTRESOURCE* res)
		{
			mResources->Insert(key, res);
		}

	public:
		ResourcePack(size_t resCount)
			: mResources(DBG_NEW Common::FTDS::HashMap<FTRESOURCE*>(resCount))
		{
		}

		~ResourcePack()
		{
			for (auto it = mResources->Begin(); it != mResources->End(); ++it)
				if (*it) delete (*it)->Value();
			delete mResources;
		}

	private:
		Common::FTDS::HashMap<FTRESOURCE*>* mResources;

	private:
		/// @brief Load single resource from .chunk
		/// @param userData Additional data necessary for resource's constructor.
		/// Pointer to renderer can be an example for graphics resources
		void LoadResource(std::ifstream& ifs, void* userData, FTRESOURCE* (*factory)(FTResourceDef&, void*))
		{
			Common::FileIOHelper::BeginDataPackLoad(ifs);

			Common::FTDS::String relPath;
			Common::FTDS::String fileName;
			Common::FileIOHelper::LoadBasicString(ifs, relPath);
			Common::FileIOHelper::LoadBasicString(ifs, fileName);

			Common::FTResourceDef resDef{fileName.C_Str(), relPath.C_Str()};
			FTRESOURCE* res = nullptr;
			if (factory) res = factory(resDef, userData);
			else if constexpr (std::is_constructible_v<FTRESOURCE, FTResourceDef&, void*>) res = DBG_NEW FTRESOURCE(resDef, userData);
			if (!res) throw std::runtime_error("Resource factory required");

			assert(0 < mResources->Capacity());
			mResources->Insert(*res->GetFileName(), res);
		}

		void LoadResource(std::ifstream& ifs)
		{
			Common::FileIOHelper::BeginDataPackLoad(ifs);

			Common::FTDS::String relPath;
			Common::FTDS::String fileName;
			Common::FileIOHelper::LoadBasicString(ifs, relPath);
			Common::FileIOHelper::LoadBasicString(ifs, fileName);

			Common::FTResourceDef resDef{fileName.C_Str(), relPath.C_Str()};
			FTRESOURCE* res = DBG_NEW FTRESOURCE(resDef);

			assert(0 < mResources->Capacity());
			mResources->Insert(*res->GetFileName(), res);
		}

#ifdef FOXTROT_EDITOR
		/// @brief Save a list of resources added to this ResourcePack to .chunk
		void SaveResourcesToChunk(std::ofstream& ofs)
		{
			Common::FileIOHelper::BeginDataPackSave(ofs, FTRESOURCE::GetName());
			for (auto iter = mResources->Begin(); iter != mResources->End(); ++iter)
				(*iter)->Value()->SaveProperties(ofs);
			Common::FileIOHelper::EndDataPackSave(ofs, FTRESOURCE::GetName());
		}

		void LoadAllResourcesInAsset(const char* dir, const char* fileType, void* userData = nullptr)
		{
			for (const std::filesystem::directory_entry& entry :
				 std::filesystem::recursive_directory_iterator(dir))
			{
				std::ifstream ifs(entry.path());
				if (entry.is_regular_file())
				{
					if (entry.path().extension() == fileType)
						LoadResource(ifs, userData, nullptr);
				}
			}
		}

		void DisplayLoadedResources(const char* label)
		{
			if (ImGui::TreeNode(label))
			{
				for (auto iter = mResources->Begin(); iter != mResources->End(); ++iter)
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
#endif // FOXTROT_EDITOR
	};
} // namespace Common