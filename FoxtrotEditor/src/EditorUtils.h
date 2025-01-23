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

#include "EditorElement.h"
#include "EditorSceneManager.h"

namespace FTEditorUtils
{
	inline bool ButtonCenteredOnLine(const char* label, float alignment = 0.5f)
	{
		ImGuiStyle& style = ImGui::GetStyle();

		float size = ImGui::CalcTextSize(label).x + style.FramePadding.x * 2.0f;
		float avail = ImGui::GetContentRegionAvail().x;

		float off = (avail - size) * alignment;
		if (off > 0.0f)
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);

		return ImGui::Button(label);
	}

	template <typename TYPE>
	inline void DisplayArrayAsCombo(const char* label, TYPE* array, size_t arraySize, int& targetIdx)
	{
		const char* comboPreview = array[targetIdx].c_str();
		if (ImGui::BeginCombo(label, comboPreview))
		{
			for (size_t i = 0; i < arraySize; ++i)
			{
				if (ImGui::Selectable(array[i].c_str()))
					targetIdx = i;
			}
			ImGui::EndCombo();
		}
	}

	inline void DisplayActorSelection(const char* label, Actor* selected)
	{
		EditorScene* editorScene = EditorSceneManager::GetInstance()->GetEditorScene();
		std::vector<Actor*>* editorElems = editorScene->GetActors();
		std::string* actorNames = DBG_NEW std::string[editorScene->GetActorCount() + 1];
		actorNames[0] = "None";
		size_t idx = 1;
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
		DisplayArrayAsCombo<std::string>(label, actorNames, idx, currIdx);
		if (0 < currIdx)
			selected = FIND_EDITOR_ELEMENT(actorNames[currIdx]);
		else
			selected = nullptr;
		delete[] actorNames;
	}

	template <typename FTRESOURCE>
	inline UINT DisplayResSelection(
		const char* label,
		std::unordered_map<UINT, FTRESOURCE*>& resMap
	)
	{
		if (ImGui::Button(label))
		{
			IGFD::FileDialogConfig config;
			config.path = ".";
			config.countSelectionMax = 1;
			ImGuiFileDialog::Instance()->OpenDialog(
				"Dialog", label, ChunkKeys::TEXTURE_FORMAT_SUPPORTED, config);
			ImGui::OpenPopup(label);
		}

		UINT key = ChunkKeys::VALUE_NOT_ASSIGNED;
		if (ImGui::BeginPopupModal(label, NULL,
			ImGuiWindowFlags_MenuBar))
		{
			if (ImGui::TreeNode("Selection State: Single Selection"))
			{
				for (auto iter = resMap.begin(); iter != resMap.end();
					++iter)
				{
					if (ImGui::Selectable((*iter).second->GetFileName().c_str()))
					{
						if ((*iter).first != ChunkKeys::VALUE_NOT_ASSIGNED)
							key = (*iter).first;
					}
				}
				ImGui::TreePop();
			}
			if (ImGui::Button("Close"))
				ImGui::CloseCurrentPopup();
			ImGui::EndPopup();
		}
		return key;
	}
}