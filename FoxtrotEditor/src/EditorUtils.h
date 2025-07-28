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

#include <Windows.h>
#include <shobjidl.h> // For IFileOpenDialog
#include <ShellScalingApi.h>

#include "EditorElement.h"
#include "EditorSceneManager.h"
#include "EditorLayer.h"
#include "DirectoryHelper.h"
#include "FileSystem/FileTypes.h"
#include "FileSystem/NullKeys.h"

#include "Dynamic/DynamicArray.h"
#include "static/HashMap.h"

namespace FTEditorUtils
{
	inline void DisplayOpenFileDialog(const COMDLG_FILTERSPEC* fileTypes, FTDS::DynamicArray<FTDS::String*>* openFileNames)
	{
		SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

		IShellItemArray* pResults;
		IFileOpenDialog* pFileOpen = nullptr;

		// Create the FileOpenDialog object
		HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pFileOpen));
		pFileOpen->SetFileTypes(GetArrayLength(fileTypes), fileTypes);

		if (SUCCEEDED(hr))
		{
			// Set options: allow multi-select, forcing file system items, etc.
			DWORD options;
			pFileOpen->GetOptions(&options);
			pFileOpen->SetOptions(options | FOS_PATHMUSTEXIST | FOS_FILEMUSTEXIST | FOS_ALLOWMULTISELECT);

			// Show the dialog
			hr = pFileOpen->Show(nullptr);
			if (SUCCEEDED(hr))
			{
				hr = pFileOpen->GetResults(&pResults);
				if (SUCCEEDED(hr))
				{
					DWORD count = 0;
					pResults->GetCount(&count);

					for (DWORD i = 0; i < count; ++i)
					{
						IShellItem* pItem;
						hr = pResults->GetItemAt(i, &pItem);
						if (SUCCEEDED(hr) && pItem)
						{
							// Retrieve the file system path
							PWSTR pszFilePath = nullptr;
							hr				  = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
							if (SUCCEEDED(hr) && pszFilePath)
							{
								// Use the selected file path
								openFileNames->PushBack(DBG_NEW FTDS::String(ToString(pszFilePath)));
								CoTaskMemFree(pszFilePath);
							}
						}
						pItem->Release();
					}
				}
			}
			pFileOpen->Release();
		}
	}

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
		EditorScene*				 editorScene = EditorSceneManager::GetInstance()->GetEditorScene();
		std::vector<EditorElement*>& editorElems = editorScene->GetEditorElements();
		FTDS::String* actorNames				 = DBG_NEW FTDS::String[editorElems.size() + 1];

		size_t idx = 1;
		for (EditorElement* ele : editorElems)
		{
			actorNames[idx] = ele->GetName();
			++idx;
		}

		static int currIdx;
		DisplayArrayAsCombo(label, actorNames, idx, currIdx);
		if (0 < currIdx)
			selected = FIND_ACTOR(actorNames[currIdx], nullptr);
		else
			selected = nullptr;
		delete[] actorNames;
	}

	template <typename FTRESOURCE>
	inline void DisplayResSelection(
		const char*					label,
		FTDS::HashMap<FTRESOURCE*>* resMap,
		FTDS::String&				currSelection)
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
				if (ImGui::Selectable("Not Assigned"))
					currSelection.Assign(ChunkKey::NullVal::NULL_OBJECT);

				resMap->IterateAllNodes([&](FTDS::Record<FTRESOURCE*>* node) {
					if (ImGui::Selectable(node->Key().C_Str()))
					{
						if (node->Key().NotEqual(ChunkKey::NullVal::NULL_OBJECT))
							currSelection = node->Key();
					}
				});

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