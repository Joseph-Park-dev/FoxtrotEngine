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
{
#ifdef FOXTROT_EDITOR
	std::string path = PATH_PROJECT + "\\Assets\\Premades\\";
#else
	std::string path = ".\\Assets\\Premades\\";
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
#ifdef FOXTROT_EDITOR
	mOrigin = DBG_NEW EditorElement();
#else
	mOrigin = DBG_NEW Actor();
#endif // FOXTROT_EDITOR
	std::ifstream ifs(GetRelativePath());
	std::pair<size_t, std::string> pack = FileIOHelper::BeginDataPackLoad(ifs);
	mOrigin->LoadProperties(ifs);
	mOrigin->LoadComponents(ifs);

	mIsLoaded = true;
}

// FTCore is needed for initialization
void FTPremade::AddToScene(Scene* scene, FTCore* ftCoreInst)
{
	scene->AddActor(mOrigin, mOrigin->GetActorGroup());
	mOrigin->Initialize(ftCoreInst);
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

void FTPremade::SaveProperties(std::ofstream& ofs, UINT key)
{
	FileIOHelper::BeginDataPackSave(ofs, ChunkKeys::FTPREMADE);
	FTResource::SaveProperties(ofs, key);
	FileIOHelper::EndDataPackSave(ofs, ChunkKeys::FTPREMADE);
}

UINT FTPremade::LoadProperties(std::ifstream& ifs)
{
	FileIOHelper::BeginDataPackLoad(ifs, ChunkKeys::FTPREMADE);
	UINT key = FTResource::LoadProperties(ifs);
	return key;
}

#ifdef FOXTROT_EDITOR
void FTPremade::Create(EditorElement* ele)
{
	assert(!GetRelativePath().empty());
	SetFileName(ele->GetName());
	std::string as = GetRelativePath() + GetFileName() + FTPremade_FORMAT_SUPPORTED;
	std::ofstream ofs(GetRelativePath() + GetFileName() + FTPremade_FORMAT_SUPPORTED);
	if (ofs)
	{
		FileIOHelper::BeginDataPackSave(ofs, GetFileName());
		ele->SaveComponents(ofs);
		ele->SaveProperties(ofs);
		FileIOHelper::EndDataPackSave(ofs, GetFileName());
		FileIOHelper::SaveBufferToFile(ofs);

		printf("Premade %s created to %s\n", GetFileName().c_str(), GetRelativePath().c_str());
	}
	else
		printf("ERROR: FTPremade::Create -> Failed to open file\n");
}

void FTPremade::Save(EditorElement* ele)
{
	assert(!GetRelativePath().empty());
	LogString(GetRelativePath());
	std::ofstream ofs(GetRelativePath());
	if (ofs)
	{
		FileIOHelper::BeginDataPackSave(ofs, GetFileName());
		ele->SaveComponents(ofs);
		ele->SaveProperties(ofs);
		FileIOHelper::EndDataPackSave(ofs, GetFileName());
		FileIOHelper::SaveBufferToFile(ofs);

		printf("Premade saved to %s\n", GetRelativePath().c_str());
	}
	else
		printf("ERROR: FTPremade::Create -> Failed to open file\n");
}

void FTPremade::UpdateUI()
{
	ImGui::Text(mFileName.c_str());
	ImGui::Separator();
	if (ButtonCenteredOnLine("Edit Premade"))
	{
		if(mOrigin)
			ImGui::OpenPopup("EditPremade");
	}
	if (ImGui::BeginPopupModal("EditPremade"))
	{
		if (mOrigin)
		{
			EditorElement dummyForUI = EditorElement(mOrigin);
			mOrigin->RemoveAllComponents();
			dummyForUI.SetIsFocused(true);
			dummyForUI.UpdateUI(true);
			dummyForUI.CloneTo(mOrigin);
			if (ImGui::Button("Save"))
				Save(&dummyForUI);
		}
		if (ImGui::Button("Close"))
		{
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}
void FTPremade::AddToScene(EditorScene* scene)
{
	scene->AddEditorElement(mOrigin);
}
#endif