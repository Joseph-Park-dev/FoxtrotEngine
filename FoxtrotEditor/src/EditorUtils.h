// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Editor Utilities. Mostly related to ImGui.
/// </summary>

#pragma once
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include "imgui/FileDialog/ImGuiFileDialog.h"
#include "imgui/FileDialog/imfilebrowser.h"

#include <future>

#include "EditorElement.h"
#include "EditorSceneManager.h"
#include "EditorLayer.h"
#include "DirectoryHelper.h"
#include "FileSystem/FileTypes.h"
#include "FileSystem/NullKeys.h"

#include "static/HashChainMap.h"

namespace FTEditorUtils
{
	inline bool ButtonCenteredOnLine(const char* label, float alignment = 0.5f)
	{
		ImGuiStyle& style = ImGui::GetStyle();

		float size	= ImGui::CalcTextSize(label).x + style.FramePadding.x * 2.0f;
		float avail = ImGui::GetContentRegionAvail().x;

		float off = (avail - size) * alignment;
		if (off > 0.0f)
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);

		return ImGui::Button(label);
	}

	inline void DisplayArrayAsCombo(const char* label, FTDS::String* array, size_t arraySize, int& targetIdx)
	{
		const char* comboPreview = array[targetIdx].C_Str();
		if (ImGui::BeginCombo(label, comboPreview))
		{
			for (size_t i = 0; i < arraySize; ++i)
			{
				if (ImGui::Selectable(array[i].C_Str()))
					targetIdx = i;
			}
			ImGui::EndCombo();
		}
	}

	inline void DisplayArrayAsCombo(const char* label, const char** array, size_t arraySize, int& targetIdx)
	{
		const char* comboPreview = array[targetIdx];
		if (ImGui::BeginCombo(label, comboPreview))
		{
			for (size_t i = 0; i < arraySize; ++i)
			{
				if (ImGui::Selectable(array[i]))
					targetIdx = i;
			}
			ImGui::EndCombo();
		}
	}

	inline void DisplayActorSelection(const char* label, Actor*& selected)
	{
		EditorScene*		 editorScene = EditorSceneManager::GetInstance()->GetEditorScene();
		std::vector<Actor*>* editorElems = editorScene->GetActors();
		FTDS::String* actorNames		 = DBG_NEW FTDS::String[editorScene->GetActorCount() + 1];
		actorNames[0]					 = "None";
		size_t idx						 = 1;
		for (size_t i = 0; i < (size_t)ActorGroup::END; ++i)
		{
			if (0 < editorElems[i].size())
			{
				for (size_t j = 0; j < editorElems[i].size(); ++j)
				{
					actorNames[idx] = (editorElems[i][j])->GetName();
					++idx;
				}
			}
		}
		static int currIdx;
		DisplayArrayAsCombo(label, actorNames, idx, currIdx);
		if (0 < currIdx)
			selected = FIND_EDITOR_ELEMENT(actorNames[currIdx]);
		else
			selected = nullptr;
		delete[] actorNames;
	}

	template <typename FTRESOURCE>
	inline void DisplayResSelection(
		const char*						 label,
		FTDS::HashChainMap<FTRESOURCE*>* resMap,
		FTDS::String&					 currSelection)
	{
		if (ImGui::Button(label))
		{
			IGFD::FileDialogConfig config;
			config.path				 = ".";
			config.countSelectionMax = 1;
			ImGuiFileDialog::Instance()->OpenDialog(
				"Dialog", label, FileTypes::TEXTURE, config);
			ImGui::OpenPopup(label);
		}

		if (ImGui::BeginPopupModal(label, NULL, ImGuiWindowFlags_MenuBar))
		{
			if (ImGui::TreeNode("Selection State: Single Selection"))
			{
				for (auto iter = resMap->Begin(); iter != resMap->End();
					 ++iter)
				{
					if (*iter)
					{
						for (auto elem = *iter; elem != nullptr; elem = elem->GetLink())
						{
							if (ImGui::Selectable(elem->Value()->FileName().C_Str()))
							{
								if (elem->Key().NotEqual(ChunkKey::NullVal::NULL_OBJECT))
									currSelection = elem->Value()->FileName();
							}
						}
					}
					else
					{
						if (ImGui::Selectable("Not Assigned"))
							currSelection.Assign(ChunkKey::NullVal::NULL_OBJECT);
					}
				}
				ImGui::TreePop();
			}
			if (ImGui::Button("Close"))
				ImGui::CloseCurrentPopup();
			ImGui::EndPopup();
		}
	}

	inline std::string GetFilePathFromDialog(const char* key, const char* title, const char* fileTypes)
	{
		IGFD::FileDialogConfig config;
		config.path				 = ".";
		config.countSelectionMax = 1;

		ImGuiFileDialog::Instance()->OpenDialog(
			key, title, fileTypes, config);
		ImGui::OpenPopup(title);

		std::string path;
		if (ImGuiFileDialog::Instance()->Display(title))
		{

			if (ImGuiFileDialog::Instance()->IsOk())
				path = ImGuiFileDialog::Instance()->GetFilePathName();

			ImGuiFileDialog::Instance()->Close();
		}
		return path;
	}
} // namespace FTEditorUtils