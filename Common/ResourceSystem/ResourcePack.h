#pragma once
#include <fstream>

#include "FTDS/Static/HashMap.h"
#include "FileSystem/FileIOHelper.h"
#include "Manager/DirectoryHelper.h"

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
		template <typename FTRESOURCE>
		void LoadResourcesFromChunk(std::ifstream& ifs, size_t& resCount, void* userData = nullptr)
		{
			FileIOHelper::BeginDataPackLoad(ifs);
			if (resCount < 1)
				return;

			mResources->Reserve(resCount);
			while (0 < resCount)
			{
				LoadResource<FTRESOURCE>(ifs, userData);
				--resCount; // Key of the next resource to be imported.
			}
		}

	public:
		FTDS::HashMap<FTRESOURCE*>* GetResMap()
		{
			return mResources;
		}

		FTRESOURCE* GetResource(FTDS::String& key)
		{
			if (key.Equal(ChunkKey::NullVal::NULL_OBJECT))
				return nullptr;

			FTDS::Record<FTRESOURCE*>* rec = mResources->At(key);
			if (!rec)
			{
				Debug::LogError(__LINE__, __FILE__, "Resource is NULL");
				return nullptr;
			}
			return rec->Value();
		}

		FTRESOURCE* GetResource(const char* key)
		{
			if (key.Equal(ChunkKey::NullVal::NULL_OBJECT))
				return nullptr;

			FTDS::Record<FTRESOURCE*>* rec = mResources->At(key);
			if (!rec)
			{
				Debug::LogError(__LINE__, __FILE__, "Resource is NULL");
				return nullptr;
			}
			return rec->Value();
		}

	public:
		ResourcePack(size_t resCount)
			: mResources(DBG_NEW FTDS::HashMap<FTRESOURCE*>(resCount))
		{
		}

		~ResourcePack()
		{
			FTDS::Safe_Delete_Map(mResources);
		}

	private:
		FTDS::HashMap<FTRESOURCE*>* mResources;

	private:
		/// @brief Load single resource from .chunk
		/// @param userData Additional data necessary for resource's constructor.
		/// Pointer to renderer can be an example for graphics resources
		void LoadResource(std::ifstream& ifs, void* userData = nullptr)
		{
			FileIOHelper::BeginDataPackLoad(ifs);

			FTDS::String relPath;
			FTDS::String fileName;
			FileIOHelper::LoadBasicString(ifs, relPath);
			FileIOHelper::LoadBasicString(ifs, fileName);

			FTResourceDef resDef(fileName, relPath);
			FTRESOURCE* res = DBG_NEW FTRESOURCE(resDef, userData);

			assert(0 < mResources->Capacity());
			mResources->Insert(*res->GetFileName(), res);
		}

#ifdef FOXTROT_EDITOR
		/// @brief Save a list of resources added to this ResourcePack to .chunk
		void SaveResourcesToChunk(std::ofstream& ofs)
		{
			FileIOHelper::BeginDataPackSave(ofs, FTRESOURCE::GetName());
			for (auto iter = mResources->Begin(); iter != mResources->End(); ++iter)
				(*iter)->Value()->SaveProperties(ofs);
			FileIOHelper::EndDataPackSave(ofs, FTRESOURCE::GetName());
		}

		void LoadAllResourcesInAsset(const char* fileType, void* userData = nullptr)
		{
			const char*	  pathToAsset = DirectoryHelper::GetInstance()->GetAssetPath().C_Str();
			std::ifstream ifs(pathToAsset);
			DirectoryHelper::IterateForFileRecurse(
				pathToAsset,
				[&](const char* itemPath) {
					FTDS::String path(itemPath);
					if (path.Contains(fileType))
						LoadResource(ifs, userData);
				});

			LoadDefaultResources();
		}
#endif // FOXTROT_EDITOR
	};
} // namespace Common