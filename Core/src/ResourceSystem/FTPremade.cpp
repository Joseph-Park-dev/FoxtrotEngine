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
	#include <imgui/imgui.h>

	#include "EditorUtils.h"
	#include "EditorSceneManager.h"
	#include "EditorChunkLoader.h"
	#include "EditorScene.h"
	#include "DirectoryHelper.h"
	#include "EditorElement.h"
#endif // FOXTROT_EDITOR

namespace Core
{
	FTPremade::FTPremade(FTResourceDef& resDef)
		: CoreResource(resDef)
		, mOrigin(nullptr)
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
	void FTPremade::Save(FTResourceDef& resDef, Editor::EditorElement* ele)
	{
		assert(resDef.Path);
		printf(resDef.Path);

		std::ofstream ofs(resDef.Path);
		if (ofs)
		{
			Core::FileIOHelper::BeginDataPackSave(ofs, resDef.FileName);
			ele->SaveComponents(ofs);
			ele->SaveProperties(ofs);
			Core::FileIOHelper::EndDataPackSave(ofs, resDef.FileName);
			Core::FileIOHelper::SaveBufferToFile(ofs);

			printf("Premade saved to %s\n", resDef.Path);
		}
		else
			printf("ERROR: FTPremade::Create -> Failed to open file\n");
	}

	void FTPremade::UpdateUI()
	{
		ImGui::Text(GetFileName()->C_Str());
		ImGui::Separator();

		if (ImGui::Button("Add to Chunk"))
		{
			Editor::EditorChunkLoader::GetInstance()->AddMaxActorID();
			int id = Editor::EditorChunkLoader::GetInstance()->GetMaxActorID();
			Editor::EditorSceneManager::GetInstance()->GetEditorScene()->AddEditorElement(mOrigin, id);
		}

		if (Editor::ButtonCenteredOnLine("Edit Premade"))
		{
			if (mOrigin)
			{
				mDummyForUI = DBG_NEW Editor::EditorElement(mOrigin, ChunkKey::ID::INVALID);
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

	FTPremade::FTPremade(FTResourceDef& resDef, Editor::EditorElement* ele)
		: FTPremade(resDef)
	{
		assert(resDef.Path);
		std::ofstream ofs(resDef.Path);
		if (ofs)
		{
			Core::FileIOHelper::BeginDataPackSave(ofs, resDef.FileName);
			ele->SaveComponents(ofs);
			ele->SaveProperties(ofs);
			Core::FileIOHelper::EndDataPackSave(ofs, resDef.FileName);
			Core::FileIOHelper::SaveBufferToFile(ofs);

			printf("Premade %s created to %s\n", resDef.FileName, resDef.Path);
		}
		else
			printf("ERROR: FTPremade::Create -> Failed to open file %s\n", resDef.Path);
	}
#endif
} // namespace Core