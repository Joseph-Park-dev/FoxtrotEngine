// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// A singleton object which is responsible of UIs in Foxtrot Editor.
/// All Imgui UIs' Update & Render operations are processed here.
/// </summary>

#pragma once
#include <functional>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/ImGuiFileDialog/ImGuiFileDialog.h>

#include "SingletonMacro.h"
#include "EditorElement.h"
#include "CommandHistory.h"

namespace D3D11
{
	class D3D11Window;
	class D3D11InputDevice;
}

enum class ErrorType
{
	None,
	ProjectPathExists,
	ProjectPathNotEmpty,
	ChunkNotSaved,
	ProjectNotValid
};

enum class InfoType
{
	None,
	ChunkIsSaved,
	PremadeIsCreated
};

enum class FileMenuEvents
{
	None,
	NewProject,
	OpenProject,
	Save,
	SaveAs,
	Open
};

namespace Editor
{
	class Command;
	class EditorRenderer;
	class EditorLayer
	{
		SINGLETON(EditorLayer)
	public:
		void Initialize(Editor::EditorRenderer* renderer);
		void Update(float deltaTime, D3D11::D3D11Window* editorWin, D3D11::D3D11InputDevice* input, Editor::EditorRenderer* renderer);
		void Render();
		void ShutDown();

	public:
		EditorElement* FocusedEditorElement() { return mFocusedEditorElement; }
		int&		   GetActorNameIdx() { return mActorNameIdx; }
		bool		   GetUndoKeyPressed() const { return mUndoKeyPressed; }
		bool		   GetRedoKeyPressed() const { return mRedoKeyPressed; }
		bool		   GetConfirmKeyPressed() const { return mConfirmKeyPressed; }
		ErrorType	   GetErrorType() const { return mErrorType; }

		// ImGuiFileDialogFlags& GetFileSelectFlag() { return mFileSelectFlag; }

		void SetInfoType(InfoType type) { mInfoType = type; }
		void SetErrorType(ErrorType type) { mErrorType = type; }

		// Is the cursor on the viewport area?
		bool CursorOnViewport() const;

	public:
		// Pops up a message box with confirm button only.
		template <class FUNCTOR>
		void PopUpInfo(const char* title, const char* msg, FUNCTOR&& onConfirm)
		{
			ImGui::OpenPopup(title);
			// Always center this window when appearing
			ImVec2 center = ImGui::GetMainViewport()->GetCenter();
			ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

			if (ImGui::BeginPopupModal(title, NULL))
			{
				ImGui::Text(msg);
				ImGui::Separator();
				ImGui::SetItemDefaultFocus();

				if (ImGui::Button("Confirm", ImVec2(120, 0)))
				{
					onConfirm();
					ImGui::CloseCurrentPopup();
					mInfoType = InfoType::None;
				}
				if (ImGui::Button("Cancel", ImVec2(120, 0)))
				{
					ImGui::CloseCurrentPopup();
					mInfoType = InfoType::None;
				}
				ImGui::EndPopup();
			}
		}

		// Pops up a message box with confirm and cancel buttons.
		// (User needs to choose)
		template <class FUNCTOR>
		void PopUpInquiry(const char* title, const char* msg, FUNCTOR&& onConfirm, FUNCTOR&& onCancel)
		{
			ImGui::OpenPopup(title);
			// Always center this window when appearing
			ImVec2 center = ImGui::GetMainViewport()->GetCenter();
			ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

			if (ImGui::BeginPopupModal(title, NULL))
			{
				ImGui::Text(msg);
				ImGui::Separator();
				ImGui::SetItemDefaultFocus();

				if (ImGui::Button("Confirm", ImVec2(120, 0)))
					onConfirm();

				if (ImGui::Button("Cancel", ImVec2(120, 0)))
					onCancel();
				ImGui::EndPopup();
			}
		}

	private:
		Editor::EditorRenderer* mRenderer;

		int mActorNameIdx;

		bool mSaveKeyPressed;
		bool mSaveAsKeyPressed;
		bool mOpenKeyPressed;
		bool mConfirmKeyPressed;

		bool mUndoKeyPressed;
		bool mRedoKeyPressed;
		bool mDeleteKeyPressed;
		bool mDuplicateKeyPressed;

		// Copy+Paste related
		EditorElement* mFocusedEditorElement;
		void*		   mDraggedEditorElement;

		// Viewport related
		bool mIsResizingViewport;
		bool mCursorOnViewport;

		// UI event types
		InfoType	   mInfoType;
		ErrorType	   mErrorType;
		FileMenuEvents mFileMenuEvent;

	private:
		/// <summary>
		//  Renders viewport.
		//  This is execptionally placed in Update() due to its requirement
		//  to be nested in ImGUi's Frame.
		/// </summary>
		void DisplayViewport(D3D11::D3D11Window* editorWin, D3D11::D3D11InputDevice* input, Editor::EditorRenderer* renderer);

		// Displays current frame rate.
		void DisplayFrameRate();

		/// <summary>
		/// Change the GUI values such as font size.
		/// </summary>
		void AdjustGUI(bool* opened);

		// Displays menu docked at the top of the screen
		// & calls the related functions.
		void DisplayMainMenuBar();

		/// <summary>
		/// Displays Edit menu for project-wise settings.
		/// </summary>
		void DisplayEditMenu();

		// Displays menu docked at the top of the screen
		// & calls the related functions.
		void DisplayManagersMenu();

		// Displays list of EditorElements in the Scene.
		// & calls the related functions.
		void DisplayHierarchyMenu();

		// Displays a single selection in the hierarchy.
		void DisplaySelection(EditorElement* actor, size_t& index);

		void ProcessDragEvent(EditorElement* from);
		void ProcessDropEvent(EditorElement* target);

		// Recurse through the children of an element, add "val"
		// to the hierarchy levels.
		void SetHierarchyLvRecurse(EditorElement* element, int val);

		// Displays FTResources loaded to current project.
		// & calls the related functions.
		void DisplayResourceMenu();

		// Displays the focused EditorElement's info.
		// & calls the related functions.
		void DisplayInspectorMenu();

		// Viewport size is changed by dragging.
		bool SceneViewportSizeChanged(ImVec2 size);

		// Does Foxtrot Project exists in projDir?
		// std::string is used exceptionally because of readability issue.
		bool ProjectExists(std::string projDir);

		void DisplayInfoMessage();
		void PopUpInfo(const char* msg);

		void DisplayErrorMessage();
		void PopUpError(const char* title, const char* msg);

		// Called according to FileMenuEvents.
		void CreateNewProject(std::string& path);
		void OpenProject(std::string& path);
		void Save(std::string& path);
		void SaveAs(std::string& path);
		void Open(std::string& path);

	private:
		void SaveEditorConfig();
		void LoadEditorConfig();
	};

	namespace ConfigKey
	{
		constexpr const char* GUI		 = "GUI Config";
		constexpr const char* FONT_SCALE = "Font Scale";
	} // namespace ConfigKey
} // namespace Editor