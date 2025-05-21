// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "ResourceSystem/FTPremade.h"

#include "FileSystem/ChunkLoader.h"
#include "FileSystem/FileIOHelper.h"
#include "Managers/ResourceManager.h"
#include "Scenes/Scene.h"
#include "Actors/Actor.h"

#ifdef FOXTROT_EDITOR
	#define IMGUI_DEFINE_MATH_OPERATORS
	#include "imgui/FileDialog/ImGuiFileDialog.h"
	#include "imgui/FileDialog/ImGuiFileDialogConfig.h"
	#include <imgui.h>

	#include "EditorUtils.h"
	#include "EditorSceneManager.h"
	#include "EditorScene.h"
	#include "DirectoryHelper.h"
	#include "EditorElement.h"
#endif // FOXTROT_EDITOR

FTPremade::FTPremade()
	: FTResource()
	, mOrigin(nullptr)
	, mIsLoaded(false)
#ifdef FOXTROT_EDITOR
	, mDummyForUI(nullptr)
#endif // FOXTROT_EDITOR

{
#ifdef FOXTROT_EDITOR
	FTDS::String path = PATH_PROJECT + "\\Assets\\Premades\\";
#else
	FTDS::String path = ".\\Assets\\Premades\\";
#endif // FOXTROT_EDITOR
	SetRelativePath(path);
}

FTPremade::~FTPremade()
{
	delete mOrigin;
	mOrigin = nullptr;
}

void FTPremade::Load()
{
	if (!mOrigin)
		mOrigin = DBG_NEW Actor();
	std::ifstream					ifs(RelativePath().C_Str());
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
	FileIOHelper::BeginDataPackSave(ofs, ChunkKey::FTPREMADE);
	FTResource::SaveProperties(ofs);
	FileIOHelper::EndDataPackSave(ofs, ChunkKey::FTPREMADE);
}

void FTPremade::LoadProperties(std::ifstream& ifs)
{
	FileIOHelper::BeginDataPackLoad(ifs, ChunkKey::FTPREMADE);
	FTResource::LoadProperties(ifs);
}

void FTPremade::Process(FTCore* coreInst)
{
	if (this->GetIsProcessed())
		return;

	this->Load();
	// All loaded premades are included as default.
	this->AddRefCount();

	this->SetIsProcessed(true);
}

#ifdef FOXTROT_EDITOR
void FTPremade::Create(EditorElement* ele)
{
	assert(!RelativePath().IsEmpty());
	SetFileName(ele->GetName().C_Str());
	FTDS::String  fileName = RelativePath() + FileName() + FileTypes::PREMADE;
	std::ofstream ofs(fileName.C_Str());
	if (ofs)
	{
		FileIOHelper::BeginDataPackSave(ofs, FileName());
		ele->SaveComponents(ofs);
		ele->SaveProperties(ofs);
		FileIOHelper::EndDataPackSave(ofs, FileName());
		FileIOHelper::SaveBufferToFile(ofs);

		printf("Premade %s created to %s\n", FileName().C_Str(), RelativePath().C_Str());
	}
	else
		printf("ERROR: FTPremade::Create -> Failed to open file\n");
}

void FTPremade::Save(EditorElement* ele)
{
	assert(!RelativePath().IsEmpty());
	LogString(RelativePath().C_Str());
	std::ofstream ofs(RelativePath().C_Str());
	if (ofs)
	{
		FileIOHelper::BeginDataPackSave(ofs, FileName());
		ele->SaveComponents(ofs);
		ele->SaveProperties(ofs);
		FileIOHelper::EndDataPackSave(ofs, FileName());
		FileIOHelper::SaveBufferToFile(ofs);

		printf("Premade saved to %s\n", RelativePath().C_Str());
	}
	else
		printf("ERROR: FTPremade::Create -> Failed to open file\n");
}

void FTPremade::UpdateUI()
{
	ImGui::Text(FileName().C_Str());
	ImGui::Separator();
	if (FTEditorUtils::ButtonCenteredOnLine("Edit Premade"))
	{
		if (mOrigin)
		{
			mDummyForUI = DBG_NEW EditorElement(mOrigin);
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
#endif