#include "ResourceSystem/FTPremade.h"

#include "FileSystem/ChunkLoader.h"
#include "FileSystem/FileIOHelper.h"
#include "Managers/ResourceManager.h"
#include "Scenes/Scene.h"

#ifdef FOXTROT_EDITOR
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/FileDialog/ImGuiFileDialog.h"
#include "imgui/FileDialog/ImGuiFileDialogConfig.h"
#include <imgui.h>

#include "EditorUtils.h"
#include "EditorSceneManager.h"
#endif // FOXTROT_EDITOR


FTPremade::FTPremade()
	: FTResource()
	, mOrigin(nullptr)
{
	SetRelativePath("./Assets/Premades/");
}

FTPremade::~FTPremade()
{
	delete mOrigin;
}

void FTPremade::Load()
{
	mOrigin = new Actor();
	std::ifstream ifs(GetRelativePath());
	std::pair<size_t, std::string> pack = FileIOHelper::BeginDataPackLoad(ifs);
	mOrigin->LoadProperties(ifs);
	mOrigin->LoadComponents(ifs);
}

void FTPremade::AddToScene(Scene* scene, FTCore* coreInst)
{
	mOrigin->Initialize(coreInst);
	scene->AddActor(mOrigin, mOrigin->GetActorGroup());
}

#ifdef FOXTROT_EDITOR
void FTPremade::Create(EditorElement* ele)
{
	assert(!GetRelativePath().empty());
	SetFileName(ele->GetName());
	std::ofstream ofs(GetRelativePath() + GetFileName() + FTPremade_FORMAT_SUPPORTED);
	if (ofs)
	{
		FileIOHelper::BeginDataPackSave(ofs, GetFileName());
		ele->SaveComponents(ofs);
		ele->SaveProperties(ofs);
		FileIOHelper::EndDataPackSave(ofs, GetFileName());
		FileIOHelper::SaveBufferToFile(ofs);

		printf("Premade created to %s", GetRelativePath().c_str());
	}
	else
		printf("ERROR: FTPremade::Create -> Failed to open file");
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

		printf("Premade saved to %s", GetRelativePath().c_str());
	}
	else
		printf("ERROR: FTPremade::Create -> Failed to open file");
}

void FTPremade::UpdateUI()
{
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
#endif
