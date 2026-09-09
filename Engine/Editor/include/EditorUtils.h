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
#include <ImGuiFileDialog/ImGuiFileDialog.h>

#include <Windows.h>
#include <shobjidl.h> // For IFileOpenDialog

#include "EditorElement.h"
#include "EditorSceneManager.h"
#include "EditorLayer.h"
#include "EditorScene.h"
#include "FileSystem/FileTypes.h"
#include "FileSystem/NullKeys.h"

#include "FTDS/Dynamic/DynamicArray.h"
#include "FTDS/Static/HashMap.h"
#include "ResourceSystem/ResourcePack.h"

namespace Editor
{
	/// @brief Opens a native file-selection dialog and returns the selected path.
	/// @param fileTypes Supported file-type filters for the dialog.
	/// @param openFileNames Receives the selected file paths.
	inline void DisplayOpenFileDialog(const COMDLG_FILTERSPEC* fileTypes, Common::FTDS::DynamicArray<Common::FTDS::String*>* openFileNames)
	{
		IShellItemArray* pResults;
		IFileOpenDialog* pFileOpen = nullptr;

		// Create the FileOpenDialog object
		HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pFileOpen));
		pFileOpen->SetFileTypes(static_cast<UINT>(Common::GetArrayLength(fileTypes)), fileTypes);

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
								openFileNames->PushBack(DBG_NEW Common::FTDS::String(Common::ToString(pszFilePath)));
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

	/// @brief Draws an ImGui button centered within the available line width.
	/// @param label Text identifying the editor control.
	/// @param alignment Alignment used when placing the content.
	/// @return True if the operation succeeds or the tested condition holds; otherwise false.
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

	/// @brief Displays array entries as an ImGui combo-box selection.
	/// @param label Text identifying the editor control.
	/// @param array Contiguous array of elements.
	/// @param arraySize Number of elements in the array.
	/// @param targetIdx Index of the destination entry.
	inline void DisplayArrayAsCombo(const char* label, Common::FTDS::String* array, size_t arraySize, int& targetIdx)
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

	/// @brief Displays array entries as an ImGui combo-box selection.
	/// @param label Text identifying the editor control.
	/// @param array Contiguous array of elements.
	/// @param arraySize Number of elements in the array.
	/// @param targetIdx Index of the destination entry.
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

	/// @brief Draws the actor selection control and updates the selected actor.
	/// @param label Text identifying the editor control.
	/// @param selected Current selection, updated by the control.
	inline void DisplayActorSelection(const char* label, Core::IActor*& selected)
	{
		Core::Scene*							   scene	   = EditorSceneManager::GetInstance()->GetCurrentScene();
		EditorScene*							   editorScene = reinterpret_cast<EditorScene*>(scene);
		Common::FTDS::DynamicArray<Core::IActor*>* editorElems = editorScene->Actors();
		Common::FTDS::String* actorNames					   = DBG_NEW Common::FTDS::String[editorElems->GetSize() + 1];
		actorNames[0].Assign("None");
		static size_t currIdx;

		for (size_t i = 0; i < editorElems->GetSize(); ++i)
			actorNames[i + 1] = editorElems->At(i)->GetName();

		Common::FTDS::String comboPreview = actorNames[0];
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
						Core::IActor* actor =
							editorScene->FindActor(actorNames[currIdx], nullptr);
						selected = actor;
					}
				}
			}
			ImGui::EndCombo();
		}
		delete[] actorNames;
	}

	/// @brief Draws a resource selection control and updates its selection.
	/// @param label Text identifying the editor control.
	/// @param resPack Resource pack supplying selectable entries.
	/// @param currSelection Current selection, updated when another entry is chosen.
	template <typename FTRESOURCE>
	inline void DisplayResSelection(
		const char*						  label,
		Common::ResourcePack<FTRESOURCE>* resPack,
		Common::FTDS::String&			  currSelection)
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
					currSelection.Assign(Common::ChunkKey::NullVal::NULL_OBJECT);

				resPack->GetResMap()->IterateAllNodes([&](Common::FTDS::Record<FTRESOURCE>* node) {
					if (ImGui::Selectable(node->Key().C_Str()))
					{
						if (node->Key().NotEqual(Common::ChunkKey::NullVal::NULL_OBJECT))
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

	/// @brief Draws a resource selection control and updates its selection.
	/// @param label Text identifying the editor control.
	/// @param resPack Resource pack supplying selectable entries.
	/// @param selectedRes Resource selected in the editor.
	template <typename FTRESOURCE>
	inline void DisplayResSelection(
		const char*						  label,
		Common::ResourcePack<FTRESOURCE>* resPack,
		FTRESOURCE*&					  selectedRes)
	{
		if (ImGui::Button(label))
			ImGui::OpenPopup(label);

		if (ImGui::BeginPopupModal(label, NULL, ImGuiWindowFlags_MenuBar))
		{
			if (ImGui::TreeNode("Selection State: Single Selection"))
			{
				resPack->GetResMap()->IterateAllNodes([&](Common::FTDS::Record<FTRESOURCE*>* node) {
					if (ImGui::Selectable(node->Key().C_Str()))
					{
						if (node->Key().NotEqual(Common::ChunkKey::NullVal::NULL_OBJECT))
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

	/// @brief Draws a resource selection control and updates its selection.
	/// @param label Text identifying the editor control.
	/// @param resPack Resource pack supplying selectable entries.
	/// @param selectedRes Resource selected in the editor.
	template <typename FTRESOURCE, typename FILTER>
	inline void DisplayResSelection(
		const char*						  label,
		Common::ResourcePack<FTRESOURCE>* resPack,
		FTRESOURCE*&					  selectedRes)
	{
		if (ImGui::Button(label))
			ImGui::OpenPopup(label);

		if (ImGui::BeginPopupModal(label, NULL, ImGuiWindowFlags_MenuBar))
		{
			if (ImGui::TreeNode("Selection State: Single Selection"))
			{
				resPack->GetResMap()->IterateAllNodes([&](Common::FTDS::Record<FTRESOURCE*>* node) {
					if (ImGui::Selectable(node->Key().C_Str()))
					{
						if (node->Key()->NotEqual(Common::ChunkKey::NullVal::NULL_OBJECT))
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

	/// @brief Returns the file path from dialog used by this service.
	/// @param key Lookup key identifying the stored entry.
	/// @param title Window title or dialog caption.
	/// @param fileTypes Supported file-type filters for the dialog.
	/// @return Current file path from dialog.
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
