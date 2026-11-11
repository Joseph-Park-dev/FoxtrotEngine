// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "ResourceSystem/FTPremade.h"

#include <cassert>
#include <filesystem>
#include <fstream>

#include "Actor/Actor.h"
#include "Debugging/DebugFuncs.h"
#include "FileSystem/FileIOHelper.h"
#include "FTDS/Static/FTString.h"
#include "Manager/ResourceManager.h"

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

	#include "ChunkLoader.h"
	#include "EditorElement.h"
	#include "EditorScene.h"
	#include "FileSystem/DLLPath.h"
#endif // FOXTROT_EDITOR

namespace Core
{
	/// @brief Initializes a premade by loading the actor stored at resDef.Path.
	/// @param resDef Resource definition containing the filename and source path.
	/// @param data Unused. Present so resource packs can construct this type.
	/// @note Initializes the :FTPremade base or delegates to its constructor.
	FTPremade::FTPremade(Common::FTResourceDef& resDef, void* data)
		: Common::FTResource(resDef)
		, mOrigin(nullptr)
		, mIsLoaded(false)
#ifdef FOXTROT_EDITOR
		, mDummyForUI(nullptr)
		, mCreateEditorElemFunc(nullptr)
		, mGetEditorSceneFunc(nullptr)
		, mGetEditorChunkLoaderFunc(nullptr)
#endif // FOXTROT_EDITOR
	{
		(void)data;
		if (mMetaData && mMetaData->Path)
			this->Load(mMetaData->Path->C_Str());

#ifdef FOXTROT_EDITOR
		BindEditorFunctions();
#endif // FOXTROT_EDITOR
	}

	/// @brief Releases the origin actor owned by this premade.
	FTPremade::~FTPremade()
	{
#ifdef FOXTROT_EDITOR
		delete mDummyForUI;
		mDummyForUI = nullptr;
#endif // FOXTROT_EDITOR
		delete mOrigin;
		mOrigin = nullptr;
	}

	/// @brief Returns the is loaded used by this ftpremade.
	/// @return Current value of the is loaded flag.
	bool FTPremade::GetIsLoaded()
	{
		return mIsLoaded;
	}

	/// @brief Returns the origin used by this ftpremade.
	/// @return Borrowed access to the origin.
	Actor* FTPremade::GetOrigin()
	{
		if (!mOrigin)
			printf("ERROR : Origin for this premade is null");
		return mOrigin;
	}

	/// @brief Loads the actor template stored in the premade file.
	/// @param path Filesystem path of the .premade file.
	void FTPremade::Load(const char* path)
	{
		if (!path || !std::filesystem::exists(path))
			return;

		std::ifstream ifs(path);
		if (!ifs)
			return;

		if (!mOrigin)
			mOrigin = DBG_NEW Actor(Common::ChunkKey::ID::CLONE);

		Common::FileIOHelper::BeginDataPackLoad(ifs);
		mOrigin->LoadProperties(ifs);
		mOrigin->LoadComponents(ifs);
		mIsLoaded = true;
	}

	/// @brief Drops the current origin and loads it again from this premade's path.
	void FTPremade::Reload()
	{
		delete mOrigin;
		mOrigin	  = nullptr;
		mIsLoaded = false;
		if (GetRelativePath())
			Load(GetRelativePath()->C_Str());
	}

#ifdef FOXTROT_EDITOR
	/// @brief Resolves editor callbacks exported by Editor.dll.
	void FTPremade::BindEditorFunctions()
	{
		HMODULE mod = GetModuleHandleA(Common::DLLPath::EDITOR);
		if (!mod)
		{
			Common::Debug::LogError(__LINE__, __FILE__, "Editor module is null");
			return;
		}

		mCreateEditorElemFunc = reinterpret_cast<Editor::CREATE_EDITOR_ELEM>(
			GetProcAddress(mod, Editor::CREATE_EDITOR_ELEMENT_FROM_ACTOR));
		mGetEditorSceneFunc = reinterpret_cast<Editor::GET_EDITOR_SCENE>(
			GetProcAddress(mod, Editor::GET_EDITOR_SCENE_FUNC));
		mGetEditorChunkLoaderFunc = reinterpret_cast<Editor::GET_EDITOR_CHUNK_LOADER>(
			GetProcAddress(mod, Editor::GET_EDITOR_CHUNK_LOADER_FUNC));
	}

	/// @brief Writes actor to the premade file described by resDef.
	/// @param resDef Resource definition containing the filename and source path.
	/// @param actor Actor whose properties and components are stored.
	void FTPremade::Save(Common::FTResourceDef& resDef, Common::IActor* actor)
	{
		assert(resDef.Path);
		assert(actor);
		if (!resDef.Path || !actor)
			return;

		std::ofstream ofs(resDef.Path);
		if (!ofs)
		{
			Common::Debug::LogError(__LINE__, __FILE__, "Failed to open premade file");
			return;
		}

		// Components are written before properties. SaveBufferToFile reverses the
		// buffer, so Load reads properties first and components second.
		Common::FileIOHelper::BeginDataPackSave(ofs, resDef.FileName);
		actor->SaveComponents(ofs);
		actor->SaveProperties(ofs);
		Common::FileIOHelper::EndDataPackSave(ofs, resDef.FileName);
		Common::FileIOHelper::SaveBufferToFile(ofs);
	}

	/// @brief Writes actor into this premade's file and reloads the origin from that file.
	/// @param actor Actor whose properties and components are stored.
	void FTPremade::Overwrite(Common::IActor* actor)
	{
		if (!actor || !GetFileName() || !GetRelativePath())
			return;

		Common::FTResourceDef resDef{ GetFileName()->C_Str(), GetRelativePath()->C_Str() };
		Save(resDef, actor);
		Reload();
	}

	/// @brief Draws controls that place or edit this premade in the editor.
	void FTPremade::UpdateUI()
	{
		if (!mCreateEditorElemFunc || !mGetEditorSceneFunc || !mGetEditorChunkLoaderFunc)
			BindEditorFunctions();

		const char* fileName = GetFileName() ? GetFileName()->C_Str() : "";
		ImGui::TextUnformatted(fileName);
		ImGui::Separator();

		if (ImGui::Button("Add to Chunk") && mOrigin && mGetEditorChunkLoaderFunc && mGetEditorSceneFunc)
		{
			mGetEditorChunkLoaderFunc()->AddMaxActorID();
			int id = mGetEditorChunkLoaderFunc()->GetMaxActorID();
			mGetEditorSceneFunc()->AddEditorElement(mOrigin, id);
		}

		if (ImGui::Button("Edit Premade") && mOrigin && mCreateEditorElemFunc)
		{
			delete mDummyForUI;
			mDummyForUI = mCreateEditorElemFunc(mOrigin, Common::ChunkKey::ID::INVALID);
			ImGui::OpenPopup(fileName);
		}

		if (ImGui::BeginPopupModal(fileName))
		{
			if (mDummyForUI)
			{
				mDummyForUI->SetIsFocused(true);
				mDummyForUI->UpdateUI(true);
				if (ImGui::Button("Save"))
				{
					Common::FTResourceDef resDef{ GetFileName()->C_Str(), GetRelativePath()->C_Str() };
					Save(resDef, mDummyForUI);
					Reload();
				}
			}
			if (ImGui::Button("Close"))
			{
				delete mDummyForUI;
				mDummyForUI = nullptr;
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}

	/// @brief Creates or replaces a premade file and registers it with the resource manager.
	/// @param fileName File name, including the .premade extension, used as the lookup key.
	/// @param path Full path of the premade file.
	/// @param actor Actor whose properties and components are stored.
	/// @return Premade owned by the resource manager, or nullptr when the arguments are unusable.
	FTPremade* CreatePremade(const char* fileName, const char* path, Common::IActor* actor)
	{
		if (!fileName || !*fileName || !path || !*path || !actor)
			return nullptr;

		Common::ResourcePack<FTPremade>* pack = ResourceManager::GetInstance()->GetPremades();
		if (!pack || !pack->GetResMap())
			return nullptr;

		Common::FTDS::HashMap<FTPremade*>* resources = pack->GetResMap();
		if (resources->Capacity() == 0)
			resources->Reserve(8);

		Common::FTDS::String				 key(fileName);
		Common::FTDS::Record<FTPremade*>*	 existing = resources->At(key);
		if (existing && existing->Value())
		{
			Common::FTDS::String updatedPath(path);
			existing->Value()->SetRelativePath(updatedPath);
			existing->Value()->Overwrite(actor);
			return existing->Value();
		}

		Common::FTResourceDef resDef{ fileName, path };
		FTPremade*			  premade = DBG_NEW FTPremade(resDef, nullptr);
		premade->Overwrite(actor);

		const size_t sizeBefore = resources->GetSize();
		pack->AddResource(premade->GetFileName(), premade);
		if (resources->GetSize() == sizeBefore)
		{
			delete premade;
			Common::Debug::LogError(__LINE__, __FILE__, "Premade pack is full");
			return nullptr;
		}
		return premade;
	}
#endif // FOXTROT_EDITOR
} // namespace Core

#ifndef FOXTROT_EDITOR
extern "C" Core::FTPremade* CreatePremade(const char*, const char*, Common::IActor*)
{
	return nullptr;
}
#endif // FOXTROT_EDITOR
