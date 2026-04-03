// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "ResourceSystem/FTPremade.h"

#include <fstream>
#include <filesystem>

#include "FileSystem/ChunkLoader.h"
#include "FileSystem/FileIOHelper.h"
#include "Manager/ResourceManager.h"
#include "Scene/Scene.h"
#include "Actor/Actor.h"
#include "Static/FTString.h"

#ifdef FOXTROT_EDITOR
	#define IMGUI_DEFINE_MATH_OPERATORS
	#include <imgui/ImGuiFileDialog/ImGuiFileDialog.h>
	#include <imgui.h>

	#include "EditorUtils.h"
	#include "EditorSceneManager.h"
	#include "EditorChunkLoader.h"
	#include "EditorScene.h"
	#include "DirectoryHelper.h"
	#include "EditorElement.h"
#endif // FOXTROT_EDITOR

namespace Core
{
	Core::ResType FTPremade::Type = Core::ResType::PREMADE;

	FTPremade::FTPremade(FTResourceDef& resDef)
		: mOrigin(nullptr)
		, mIsLoaded(false)
#ifdef FOXTROT_EDITOR
		, mDummyForUI(nullptr)
#endif // FOXTROT_EDITOR
	{
		if (mOrigin)
			return;

		if (std::filesystem::exists(resDef.Path))
			this->Load(resDef.Path);
	}

	FTPremade::~FTPremade()
	{
		delete mOrigin;
		mOrigin = nullptr;
	}

	bool FTPremade::GetIsLoaded()
	{
		return mIsLoaded;
	}

	Actor* FTPremade::GetOrigin()
	{
		if (!mOrigin)
			printf("ERROR : Origin for this premade is null");
		return mOrigin;
	}

	void FTPremade::Load(const char* path)
	{
		if (!mOrigin)
			mOrigin = DBG_NEW Actor(ChunkKey::ID::CLONE);

		std::ifstream					ifs(path);
		std::pair<size_t, FTDS::String> pack = Core::FileIOHelper::BeginDataPackLoad(ifs);
		mOrigin->LoadProperties(ifs);
		mOrigin->LoadComponents(ifs);

		// #ifdef FOXTROT_EDITOR
		//	mOrigin->Initialize(FTCoreEditor::GetInstance());
		// #else
		//	mOrigin->Initialize(FTCore::GetInstance());
		// #endif // FOXTROT_EDITOR

		mIsLoaded = true;
	}

#ifdef FOXTROT_EDITOR
	void FTPremade::Create(EditorElement* ele)
	{
		assert(!GetRelativePath().IsEmpty());

		FTDS::String  path = GetRelativePath();
		std::ofstream ofs(path.C_Str());
		if (ofs)
		{
			Core::FileIOHelper::BeginDataPackSave(ofs, GetFileName());
			ele->SaveComponents(ofs);
			ele->SaveProperties(ofs);
			Core::FileIOHelper::EndDataPackSave(ofs, GetFileName());
			Core::FileIOHelper::SaveBufferToFile(ofs);

			printf("Premade %s created to %s\n", GetFileName().C_Str(), path.C_Str());
		}
		else
			printf("ERROR: FTPremade::Create -> Failed to open file %s\n", path.C_Str());
	}

	void FTPremade::Save(EditorElement* ele)
	{
		assert(!GetRelativePath().IsEmpty());
		LogString(GetRelativePath().C_Str());
		std::ofstream ofs(GetRelativePath().C_Str());
		if (ofs)
		{
			Core::FileIOHelper::BeginDataPackSave(ofs, GetFileName());
			ele->SaveComponents(ofs);
			ele->SaveProperties(ofs);
			Core::FileIOHelper::EndDataPackSave(ofs, GetFileName());
			Core::FileIOHelper::SaveBufferToFile(ofs);

			printf("Premade saved to %s\n", GetRelativePath().C_Str());
		}
		else
			printf("ERROR: FTPremade::Create -> Failed to open file\n");
	}

	void FTPremade::UpdateUI()
	{
		ImGui::Text(GetFileName().C_Str());
		ImGui::Separator();

		if (ImGui::Button("Add to Chunk"))
		{
			EditorChunkLoader::GetInstance()->AddMaxActorID();
			int id = EditorChunkLoader::GetInstance()->GetMaxActorID();
			EditorSceneManager::GetInstance()->GetEditorScene()->AddEditorElement(mOrigin, id);
		}

		if (FTEditorUtils::ButtonCenteredOnLine("Edit Premade"))
		{
			if (mOrigin)
			{
				mDummyForUI = DBG_NEW EditorElement(mOrigin, ChunkKey::ID::INVALID);
				ImGui::OpenPopup("EditPremade");
			}
		}
		if (ImGui::BeginPopupModal("EditPremade"))
		{
			if (mDummyForUI)
			{
				// Only values from Actor is needed here.
				// Thus dynamic_cast is not needed.
				mDummyForUI->SetIsFocused(true);
				mDummyForUI->UpdateUI(true);
				if (ImGui::Button("Save"))
					Save(mDummyForUI);
			}
			if (ImGui::Button("Close"))
			{
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}

	void FTPremade::AddRefCount()
	{
		FTResource::AddRefCount();
	}

	void FTPremade::SubtractRefCount()
	{
		FTResource::SubtractRefCount();
	}
#endif
} // namespace Core