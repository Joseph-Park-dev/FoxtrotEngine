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

FTPremade::FTPremade(FTResourceDef& resDef)
	: FTResource(resDef)
	, mOrigin(nullptr)
	, mIsLoaded(false)
#ifdef FOXTROT_EDITOR
	, mDummyForUI(nullptr)
#endif // FOXTROT_EDITOR
{
	Process();
}

FTPremade::~FTPremade()
{
	delete mOrigin;
	mOrigin = nullptr;
}

void FTPremade::Load()
{
	if (!mOrigin)
		mOrigin = DBG_NEW Actor(ChunkKey::ID::CLONE);

	std::ifstream					ifs(GetRelativePath().C_Str());
	std::pair<size_t, FTDS::String> pack = FileIOHelper::BeginDataPackLoad(ifs);
	mOrigin->LoadProperties(ifs);
	mOrigin->LoadComponents(ifs);

	// #ifdef FOXTROT_EDITOR
	//	mOrigin->Initialize(FTCoreEditor::GetInstance());
	// #else
	//	mOrigin->Initialize(FTCore::GetInstance());
	// #endif // FOXTROT_EDITOR

	mIsLoaded = true;
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

void FTPremade::SaveProperties(std::ofstream& ofs)
{
	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::FTPremade::FT_PREMADE);
	FTResource::SaveProperties(ofs);
	FileIOHelper::EndDataPackSave(ofs, ChunkKey::FTPremade::FT_PREMADE);
}

void FTPremade::LoadProperties(std::ifstream& ifs)
{
	FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FTPremade::FT_PREMADE);
	FTResource::LoadProperties(ifs);
}

void FTPremade::Process()
{
	if (IsProcessed())
		return;

	if (std::filesystem::exists(GetRelativePath().C_Str()))
		this->Load();

	// All loaded premades are included as default.
	FTResource::Process();
}

#ifdef FOXTROT_EDITOR
void FTPremade::Create(EditorElement* ele)
{
	assert(!GetRelativePath().IsEmpty());

	FTDS::String  path = GetRelativePath();
	std::ofstream ofs(path.C_Str());
	if (ofs)
	{
		FileIOHelper::BeginDataPackSave(ofs, GetFileName());
		ele->SaveComponents(ofs);
		ele->SaveProperties(ofs);
		FileIOHelper::EndDataPackSave(ofs, GetFileName());
		FileIOHelper::SaveBufferToFile(ofs);

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
		FileIOHelper::BeginDataPackSave(ofs, GetFileName());
		ele->SaveComponents(ofs);
		ele->SaveProperties(ofs);
		FileIOHelper::EndDataPackSave(ofs, GetFileName());
		FileIOHelper::SaveBufferToFile(ofs);

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