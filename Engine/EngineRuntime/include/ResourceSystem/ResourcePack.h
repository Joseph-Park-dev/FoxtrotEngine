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
	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN
	#endif
	#ifndef NOMINMAX
		#define NOMINMAX
	#endif
	#include <Windows.h>
	#include "Factory/IFTResourceFactory.h"
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
		/// @param ifs Input stream positioned at the expected data; reading advances its position.
		/// @param factory Optional factory callback used to construct a resource from its definition.
		void LoadResourcesFromChunk(std::ifstream& ifs, void* userData)
		{
			size_t resCount = Common::FileIOHelper::BeginDataPackLoad(ifs).first;
			if (resCount < 1)
				return;

			mResources->Reserve(resCount);
			while (0 < resCount)
			{
				LoadResource(ifs, userData);
				--resCount; // Key of the next resource to be imported.
			}
		}

		/// @brief Restores resource entries from a .chunk data pack.
		/// @param ifs Input stream positioned at the expected data; reading advances its position.
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
		/// @brief Returns the res map used by this resource pack.
		/// @return Borrowed access to the res map.
		Common::FTDS::HashMap<FTRESOURCE*>* GetResMap()
		{
			return mResources;
		}

		/// @brief Returns the resource used by this resource pack.
		/// @param key Lookup key identifying the stored entry.
		/// @return Borrowed access to the resource. May be null when no matching object is available.
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

		/// @brief Returns the resource used by this resource pack.
		/// @param key Lookup key identifying the stored entry.
		/// @return Borrowed access to the resource. May be null when no matching object is available.
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

		/// @brief Registers a resource in the pack under the supplied lookup key.
		/// @param key Lookup key identifying the stored entry.
		/// @param res Resource to register or inspect.
		void AddResource(const Common::FTDS::String* key, FTRESOURCE* res)
		{
			mResources->Insert(*key, res);
		}

		/// @brief Registers a resource in the pack under the supplied lookup key.
		/// @param key Lookup key identifying the stored entry.
		/// @param res Resource to register or inspect.
		void AddResource(const char* key, FTRESOURCE* res)
		{
			mResources->Insert(key, res);
		}

	public:
		/// @brief Initializes an empty resource registry with the requested capacity.
		/// @param resCount Initial capacity of the resource registry.
		ResourcePack(size_t resCount)
			: mResources(DBG_NEW Common::FTDS::HashMap<FTRESOURCE*>(resCount))
		{
		}

		/// @brief Releases the resources managed by this instance during destruction.
		~ResourcePack()
		{
			static_assert(sizeof(FTRESOURCE) > 0, "ResourcePack destruction requires a complete resource type");
			for (auto it = mResources->Begin(); it != mResources->End(); ++it)
				if (*it)
					delete (*it)->Value();
			delete mResources;
		}

	private:
		Common::FTDS::HashMap<FTRESOURCE*>* mResources;

	private:
		/// @brief Load single resource from .chunk
		/// @param userData Additional data necessary for resource's constructor.
		/// Pointer to renderer can be an example for graphics resources
		/// @param ifs Input stream positioned at the expected data; reading advances its position.
		/// @param factory Optional factory callback used to construct a resource from its definition.
		/// @throws std::runtime_error If the operation encounters the failure condition checked by this implementation.
		void LoadResource(std::ifstream& ifs, void* userData = nullptr)
		{
			Common::FileIOHelper::BeginDataPackLoad(ifs);

			Common::FTDS::String relPath;
			Common::FTDS::String fileName;
			Common::FileIOHelper::LoadBasicString(ifs, relPath);
			Common::FileIOHelper::LoadBasicString(ifs, fileName);

			Common::FTResourceDef resDef{ fileName.C_Str(), relPath.C_Str() };

			FTRESOURCE* res = DBG_NEW FTRESOURCE(resDef, userData);
			if (!res)
				Common::Debug::LogError(__LINE__, __FILE__, "Resource is NULL");
			assert(0 < mResources->Capacity());
			mResources->Insert(*res->GetFileName(), res);
		}

		void LoadResource(Common::FTDS::String&& relPath, void* userData = nullptr)
		{
			Common::FTDS::String  fileName = ExtractFileName(relPath.C_Str());
			Common::FTResourceDef resDef{ fileName.C_Str(), relPath.C_Str() };

			FTRESOURCE* res = DBG_NEW FTRESOURCE(resDef, userData);
			if (!res)
				Common::Debug::LogError(__LINE__, __FILE__, "Resource is NULL");
			assert(0 < mResources->Capacity());
			mResources->Insert(*res->GetFileName(), res);
		}

#ifdef FOXTROT_EDITOR
	public:
		/// @brief Save a list of resources added to this ResourcePack to .chunk
		/// @param ofs Output stream receiving the serialized data.
		void SaveResourcesToChunk(std::ofstream& ofs)
		{
			Common::FileIOHelper::BeginDataPackSave(ofs, FTRESOURCE::GetName());
			for (auto iter = mResources->Begin(); iter != mResources->End(); ++iter)
				if (*iter && (*iter)->Value())
					(*iter)->Value()->SaveProperties(ofs);
			Common::FileIOHelper::EndDataPackSave(ofs, FTRESOURCE::GetName());
		}

		/// @brief Saves resources whose metadata records an active reference.
		/// @param ofs Output stream receiving the serialized data.
		void SaveReferencedResourcesToChunk(std::ofstream& ofs)
		{
			Common::FileIOHelper::BeginDataPackSave(ofs, FTRESOURCE::GetName());
			for (auto iter = mResources->Begin(); iter != mResources->End(); ++iter)
			{
				if (!*iter || !(*iter)->Value())
					continue;
				if ((*iter)->Value()->IsReferenced())
					(*iter)->Value()->SaveProperties(ofs);
			}
			Common::FileIOHelper::EndDataPackSave(ofs, FTRESOURCE::GetName());
		}

		/// @brief Restores resources recorded by SaveReferencedResourcesToChunk and marks them referenced.
		/// @param ifs Input stream positioned at the pack header; reading advances through that pack.
		/// @param module Module that exports SetResourceFactory for this pack's resource types.
		/// @param renderer Renderer forwarded to graphics resource factories. Core resources pass nullptr.
		void LoadReferencedResourcesFromChunk(std::ifstream& ifs, HMODULE module, void* renderer = nullptr)
		{
			// Save writes the file name then the path, and the buffer is stored in reverse.
			// FTResource::LoadProperties reads that pair as path, then file name.
			struct EntryReader
			{
				Common::FTDS::HashMap<FTRESOURCE*>*			 Resources;
				Common::FTDS::HashMap<ResourceFactoryEntry>* Factories;
				ResourceFactoryEntry						 PackFactory;
				void*										 Renderer;

				EntryReader(
					Common::FTDS::HashMap<FTRESOURCE*>*			 resources,
					Common::FTDS::HashMap<ResourceFactoryEntry>* factories,
					ResourceFactoryEntry						 packFactory,
					void*										 renderer)
					: Resources(resources)
					, Factories(factories)
					, PackFactory(packFactory)
					, Renderer(renderer)
				{
				}

				bool NextIsDataPack(std::ifstream& stream)
				{
					if (stream.fail())
						stream.clear();
					const std::streampos pos = stream.tellg();
					Common::FTDS::String line;
					const bool			 got = Common::FileIOHelper::GetLine(stream, line);
					if (stream.fail())
						stream.clear();
					stream.seekg(pos);
					if (!got)
						return false;
					return line.Contains("<") && line.Contains(">");
				}

				void Rewind(std::ifstream& stream, std::streampos pos)
				{
					if (stream.fail())
						stream.clear();
					stream.seekg(pos);
				}

				bool ReadFieldHeader(std::ifstream& stream, Common::FTDS::String& key, bool& isString)
				{
					Common::FTDS::String line;
					if (!Common::FileIOHelper::GetLine(stream, line))
						return false;
					line.ExtractUntilLast(key, "[");
					key.ExtractFromLast("\t");
					isString = line.Contains("[string]");
					Common::FTDS::String value;
					Common::FileIOHelper::GetLine(stream, value);
					return true;
				}

				bool IsIdentityPair(const Common::FTDS::String& first, const Common::FTDS::String& second) const
				{
					const bool pathThenName =
						first.Equal(Common::ChunkKey::FTResource::RELATIVE_PATH) &&
						second.Equal(Common::ChunkKey::FTResource::FILE_NAME);
					const bool nameThenName =
						first.Equal(Common::ChunkKey::FTResource::FILE_NAME) &&
						second.Equal(Common::ChunkKey::FTResource::FILE_NAME);
					return pathThenName || nameThenName;
				}

				bool TakeIdentity(
					std::ifstream&		  stream,
					size_t				  remaining,
					Common::FTDS::String& fileName,
					Common::FTDS::String& path)
				{
					if (remaining < 2 || NextIsDataPack(stream))
						return false;

					if (stream.fail())
						stream.clear();
					const std::streampos start = stream.tellg();
					Common::FTDS::String first;
					Common::FTDS::String second;
					bool				 firstString  = false;
					bool				 secondString = false;
					if (!ReadFieldHeader(stream, first, firstString) ||
						NextIsDataPack(stream) ||
						!ReadFieldHeader(stream, second, secondString) ||
						!firstString ||
						!secondString ||
						!IsIdentityPair(first, second))
					{
						Rewind(stream, start);
						return false;
					}

					Rewind(stream, start);
					Common::FTResource probe;
					probe.FTResource::LoadProperties(stream);
					if (probe.GetFileName())
						fileName.Assign(*probe.GetFileName());
					if (probe.GetRelativePath())
						path.Assign(*probe.GetRelativePath());
					return true;
				}

				void NoteField(std::ifstream& stream, Common::FTDS::String& fileName, Common::FTDS::String& path)
				{
					if (stream.fail())
						stream.clear();
					const std::streampos pos = stream.tellg();
					Common::FTDS::String line;
					if (!Common::FileIOHelper::GetLine(stream, line))
						return;

					const bool			 isString = line.Contains("[string]");
					Common::FTDS::String key;
					line.ExtractUntilLast(key, "[");
					key.ExtractFromLast("\t");
					if (!isString)
					{
						Common::FTDS::String discard;
						Common::FileIOHelper::GetLine(stream, discard);
						return;
					}

					Rewind(stream, pos);
					Common::FTDS::String value;
					Common::FileIOHelper::LoadBasicString(stream, value);
					if (key.Equal(Common::ChunkKey::FTResource::RELATIVE_PATH) && path.IsEmpty())
						path.Assign(value);
					else if (key.Equal(Common::ChunkKey::FTResource::FILE_NAME) && fileName.IsEmpty())
						fileName.Assign(value);
				}

				void ReadItems(
					std::ifstream&		  stream,
					size_t				  count,
					Common::FTDS::String& fileName,
					Common::FTDS::String& path)
				{
					size_t index = 0;
					while (index < count)
					{
						if (NextIsDataPack(stream))
						{
							const size_t nestedCount = Common::FileIOHelper::BeginDataPackLoad(stream).first;
							ReadItems(stream, nestedCount, fileName, path);
							++index;
							continue;
						}
						if (TakeIdentity(stream, count - index, fileName, path))
						{
							index += 2;
							continue;
						}
						NoteField(stream, fileName, path);
						++index;
					}
				}

				void Keep(
					const Common::FTDS::String& fileName,
					const Common::FTDS::String& path,
					ResourceFactoryEntry		factory)
				{
					if (fileName.IsEmpty())
						return;

					Common::FTDS::Record<FTRESOURCE*>* existing = Resources->At(fileName);
					if (existing && existing->Value())
					{
						existing->Value()->AddRefCount();
						return;
					}

					if (!factory)
					{
						Common::Debug::LogError(__LINE__, __FILE__, "Resource factory is NULL");
						return;
					}

					Common::FTResourceDef resDef{ fileName.C_Str(), path.C_Str() };
					Common::IResource*	  createdBase = factory(resDef, Renderer);
					if (!createdBase)
					{
						Common::Debug::LogError(__LINE__, __FILE__, "Resource is NULL");
						return;
					}

					FTRESOURCE* created = static_cast<FTRESOURCE*>(createdBase);
					Common::FTDS::String* createdName = created->GetFileName();
					if (!createdName)
					{
						delete created;
						return;
					}

					assert(0 < Resources->Capacity());
					Resources->Insert(*createdName, created);
					Common::FTDS::Record<FTRESOURCE*>* stored = Resources->At(*createdName);
					if (!stored || stored->Value() != created)
					{
						delete created;
						return;
					}
					created->AddRefCount();
				}

				ResourceFactoryEntry FactoryFor(const Common::FTDS::String& key) const
				{
					if (!Factories)
						return PackFactory;
					Common::FTDS::Record<ResourceFactoryEntry>* record = Factories->At(key);
					if (record && record->Value())
						return record->Value();
					return PackFactory;
				}

				void ReadNested(std::ifstream& stream)
				{
					const std::pair<size_t, Common::FTDS::String> loaded =
						Common::FileIOHelper::BeginDataPackLoad(stream);
					Common::FTDS::String fileName;
					Common::FTDS::String path;
					ReadItems(stream, loaded.first, fileName, path);
					Keep(fileName, path, FactoryFor(loaded.second));
				}

				void ReadFlat(std::ifstream& stream)
				{
					Common::FTResource probe;
					probe.FTResource::LoadProperties(stream);
					Common::FTDS::String fileName;
					Common::FTDS::String path;
					if (probe.GetFileName())
						fileName.Assign(*probe.GetFileName());
					if (probe.GetRelativePath())
						path.Assign(*probe.GetRelativePath());
					Keep(fileName, path, PackFactory);
				}

				void SkipField(std::ifstream& stream)
				{
					Common::FTDS::String discard;
					Common::FileIOHelper::GetLine(stream, discard);
					Common::FileIOHelper::GetLine(stream, discard);
				}
			};

			size_t count = Common::FileIOHelper::BeginDataPackLoad(ifs, FTRESOURCE::GetName()).first;
			if (count < 1)
				return;

			if (mResources->GetSize() == 0 && mResources->Capacity() < count)
				mResources->Reserve(count);

			using SetResourceFactoryFn = void(__cdecl*)(Common::FTDS::HashMap<ResourceFactoryEntry>&);
			SetResourceFactoryFn setFactory = nullptr;
			if (!module)
			{
				Common::Debug::LogError(__LINE__, __FILE__, "Resource factory module is NULL");
			}
			else
			{
				setFactory = reinterpret_cast<SetResourceFactoryFn>(GetProcAddress(module, "SetResourceFactory"));
				if (!setFactory)
					Common::Debug::LogError(__LINE__, __FILE__, "SetResourceFactory is NULL");
			}

			Common::FTDS::HashMap<ResourceFactoryEntry> entries(32);
			ResourceFactoryEntry						packFactory = nullptr;
			Common::FTDS::HashMap<ResourceFactoryEntry>* factories	 = nullptr;
			if (setFactory)
			{
				setFactory(entries);
				factories = &entries;
				Common::FTDS::Record<ResourceFactoryEntry>* record = entries.At(FTRESOURCE::GetName());
				if (record)
					packFactory = record->Value();
			}

			EntryReader reader(mResources, factories, packFactory, renderer);
			while (0 < count)
			{
				if (reader.NextIsDataPack(ifs))
				{
					reader.ReadNested(ifs);
					--count;
					continue;
				}
				if (count < 2)
				{
					reader.SkipField(ifs);
					break;
				}
				reader.ReadFlat(ifs);
				count -= 2;
			}
		}

		/// @brief Visits asset files of the requested type and loads them into this resource pack.
		/// @param dir Directory to enumerate or resolve.
		/// @param fileType File extension or type used to filter resources.
		/// @param userData Additional context forwarded to resource construction.
		void LoadAllResourcesInAsset(const char* dir, const char* fileType, void* userData = nullptr)
		{
			if (!dir || !*dir || !fileType || !*fileType)
				return;
			if (!std::filesystem::exists(dir) || !std::filesystem::is_directory(dir))
				return;

			size_t count = 0;
			for (const std::filesystem::directory_entry& entry :
				 std::filesystem::recursive_directory_iterator(dir))
			{
				if (!entry.is_regular_file())
					continue;
				FTDS::String accepted(fileType);
				if (accepted.Contains(entry.path().extension().string().c_str()))
					++count;
			}
			if (count < 1)
				return;

			// Empty packs are created with capacity 0. Size them before the first insert.
			// A pack that already holds records is left as-is: growing it clears every slot.
			if (mResources->GetSize() == 0 && mResources->Capacity() < count)
				mResources->Reserve(count);

			for (const std::filesystem::directory_entry& entry :
				 std::filesystem::recursive_directory_iterator(dir))
			{
				if (!entry.is_regular_file())
					continue;
				FTDS::String accepted(fileType);
				if (accepted.Contains(entry.path().extension().string().c_str()))
					LoadResource(entry.path().string().c_str(), userData);
			}
		}

		/// @brief Draws the editor's resource selection controls for this pack.
		/// @param label Text identifying the editor control.
		void DisplayLoadedResources(const char* label)
		{
			if (ImGui::TreeNode(label))
			{
				for (auto iter = mResources->Begin(); iter != mResources->End(); ++iter)
				{
					if (!*iter || !(*iter)->Value())
						continue;
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
