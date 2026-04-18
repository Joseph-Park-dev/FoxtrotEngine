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
#include <imgui/imgui.h>
#include "imgui/ImGuiFileDialog/ImGuiFileDialog.h"

#include <Windows.h>
#include <shobjidl.h> // For IFileOpenDialog

#include "EditorElement.h"
#include "EditorSceneManager.h"
#include "EditorLayer.h"
#include "EditorScene.h"
#include "DirectoryHelper.h"
#include "FileSystem/FileTypes.h"
#include "FileSystem/NullKeys.h"

#include "FTDS/Dynamic/DynamicArray.h"
#include "FTDS/Static/HashMap.h"

namespace Editor
{
	inline void DisplayOpenFileDialog(const COMDLG_FILTERSPEC* fileTypes, Core::FTDS::DynamicArray<Core::FTDS::String*>* openFileNames)
	{
		IShellItemArray* pResults;
		IFileOpenDialog* pFileOpen = nullptr;

		// Create the FileOpenDialog object
		HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pFileOpen));
		pFileOpen->SetFileTypes(static_cast<UINT>(Core::GetArrayLength(fileTypes)), fileTypes);

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
								openFileNames->PushBack(DBG_NEW Core::FTDS::String(Core::ToString(pszFilePath)));
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

	inline void DisplayArrayAsCombo(const char* label, Core::FTDS::String* array, size_t arraySize, int& targetIdx)
	{
		const char* comboPreview = array[targetIdx].C_Str();
		if (ImGui::BeginCombo(label, comboPreview))
		{
			for (size_t i = 0; i < arraySize; ++i)
			{
				if (ImGui::Selectable(array[i].C_Str()))
					targetIdx = static_cast<int>(i);
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
					targetIdx = static_cast<int>(i);
			}
			ImGui::EndCombo();
		}
	}

	inline void DisplayActorSelection(const char* label, Core::Actor*& selected)
	{
		Core::Scene*							scene		= EditorSceneManager::GetInstance()->GetCurrentScene();
		EditorScene*							editorScene = reinterpret_cast<EditorScene*>(scene);
		Core::FTDS::DynamicArray<Core::Actor*>* editorElems = editorScene->Actors();
		Core::FTDS::String* actorNames						= DBG_NEW Core::FTDS::String[editorElems->GetSize() + 1];
		actorNames[0].Assign("None");
		static size_t currIdx;

		for (size_t i = 0; i < editorElems->GetSize(); ++i)
			actorNames[i + 1] = editorElems->At(i)->GetName();

		Core::FTDS::String comboPreview = actorNames[0];
		if (selected)
			comboPreview = selected->GetName();
		if (ImGui::BeginCombo("Actor Selection", comboPreview.C_Str()))
		{
			for (size_t i = 0; i < editorElems->GetSize() + 1; ++i)
			{
				if (ImGui::Selectable(actorNames[i].C_Str()))
				{
					currIdx = i;
					if (currIdx == 0)
						selected = nullptr;
					else
					{
						Core::Actor* actor =
							editorScene->FindActor(actorNames[currIdx], nullptr);
						selected = actor;
					}
				}
			}
			ImGui::EndCombo();
		}
		delete[] actorNames;
	}

	template <typename FTRESOURCE>
	inline void DisplayResSelection(
		const char*						  label,
		Core::FTDS::HashMap<FTRESOURCE*>* resMap,
		Core::FTDS::String&				  currSelection)
	{
		if (ImGui::Button(label))
		{
			IGFD::FileDialogConfig config;
			config.path				 = ".";
			config.countSelectionMax = 1;
			ImGuiFileDialog::Instance()->OpenDialog(
				"Dialog", label, nullptr, config);
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

	template <typename FTRESOURCE>
	inline void DisplayResSelection(
		const char*						  label,
		Core::FTDS::HashMap<FTRESOURCE*>* resMap,
		FTRESOURCE*&					  selectedRes)
	{
		if (ImGui::Button(label))
			ImGui::OpenPopup(label);

		if (ImGui::BeginPopupModal(label, NULL, ImGuiWindowFlags_MenuBar))
		{
			if (ImGui::TreeNode("Selection State: Single Selection"))
			{
				resMap->IterateAllNodes([&](FTDS::Record<FTRESOURCE*>* node) {
					if (ImGui::Selectable(node->Key().C_Str()))
					{
						if (node->Key().NotEqual(ChunkKey::NullVal::NULL_OBJECT))
						{
							if (selectedRes)
								selectedRes->SubtractRefCount();

							selectedRes = node->Value();
							selectedRes->AddRefCount();
						}
					}
				});

				ImGui::TreePop();
			}
			if (ImGui::Button("Close"))
				ImGui::CloseCurrentPopup();
			ImGui::EndPopup();
		}
	}

	template <typename FTRESOURCE, typename FILTER>
	inline void DisplayResSelection(
		const char*						  label,
		Core::FTDS::HashMap<FTRESOURCE*>* resMap,
		FTRESOURCE*&					  selectedRes)
	{
		if (ImGui::Button(label))
			ImGui::OpenPopup(label);

		if (ImGui::BeginPopupModal(label, NULL, ImGuiWindowFlags_MenuBar))
		{
			if (ImGui::TreeNode("Selection State: Single Selection"))
			{
				resMap->IterateAllNodes([&](FTDS::Record<FTRESOURCE*>* node) {
					if (ImGui::Selectable(node->Key().C_Str()))
					{
						if (node->Key().NotEqual(ChunkKey::NullVal::NULL_OBJECT))
						{
							if (typeid(node->Value()) == typeid(FILTER))
							{
								if (selectedRes)
									selectedRes->SubtractRefCount();

								selectedRes = node->Value();
								selectedRes->AddRefCount();
							}
						}
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
} // namespace Editor