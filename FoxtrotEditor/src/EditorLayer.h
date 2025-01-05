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
#include <imgui.h>
#include "imgui/FileDialog/imfilebrowser.h"

#include "Core/SingletonMacro.h"
#include "EditorElement.h"
#include "CommandHistory.h"

class FoxtrotRenderer;
class UIActor;
class PanelUI;
class Command;

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

class EditorLayer
{
	SINGLETON(EditorLayer);

public:
	void Update(float deltaTime);
	void Render(FoxtrotRenderer* renderer);
	void ShutDown();

public:
	int&		GetActorNameIdx()			  { return mActorNameIdx; }
	bool		GetUndoKeyPressed()		const { return mUndoKeyPressed; }
	bool		GetRedoKeyPressed()		const { return mRedoKeyPressed; }
	bool		GetConfirmKeyPressed()	const { return mConfirmKeyPressed; }
	ImVec2		GetSceneViewportPos()	const { return mSceneViewportPos; }
	ImVec2		GetSceneViewportSize()	const { return mSceneViewportSize; }
	ErrorType	GetErrorType()			const { return mErrorType; }

	void		SetInfoType(InfoType type) { mInfoType = type; }
	void		SetErrorType(ErrorType type) { mErrorType = type; }

	// Is the cursor on the viewport area?
	bool		CursorOnViewport() const; 

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
	int			mHierarchyIdx;
	int			mActorNameIdx;

	bool		mSaveKeyPressed;
	bool		mSaveAsKeyPressed;
	bool		mOpenKeyPressed;
	bool		mConfirmKeyPressed;

	bool		mUndoKeyPressed;
	bool		mRedoKeyPressed;
	bool		mDeleteKeyPressed;
	bool		mDuplicateKeyPressed;

	// Copy+Paste related
	EditorElement*	mFocusedEditorElement;

	// Viewport related
	ImVec2			mSceneViewportPos;
	ImVec2			mSceneViewportSize;
	bool			mIsResizingViewport;
	bool			mCursorOnViewport;

	// UI event types
	InfoType		mInfoType;
	ErrorType		mErrorType;
	FileMenuEvents	mFileMenuEvent;

	ImGui::FileBrowser mFileDialog;
	ImGuiFileBrowserFlags mDirSelectFlag =
		ImGuiFileBrowserFlags_SelectDirectory |
		ImGuiFileBrowserFlags_HideRegularFiles |
		ImGuiFileBrowserFlags_ConfirmOnEnter |
		ImGuiFileBrowserFlags_CloseOnEsc |
		ImGuiFileBrowserFlags_CreateNewDir |
		ImGuiFileBrowserFlags_EditPathString;

	ImGuiFileBrowserFlags mFileSelectFlag =
		ImGuiFileBrowserFlags_EnterNewFilename |
		ImGuiFileBrowserFlags_ConfirmOnEnter |
		ImGuiFileBrowserFlags_CloseOnEsc |
		ImGuiFileBrowserFlags_CreateNewDir |
		ImGuiFileBrowserFlags_EditPathString;

private:
	/// <summary>
	//  Renders viewport. 
	//  This is execptionally placed in Update() due to its requirement 
	//  to be nested in ImGUi's Frame.
	/// </summary>
	void DisplayViewport();

	// Displays menu docked at the top of the screen
	// & calls the related functions.
	void DisplayFileMenu();

	// Displays list of EditorElements in the Scene.
	// & calls the related functions.
	void DisplayHierarchyMenu();

	// Displays FTResources loaded to current project.
	// & calls the related functions.
	void DisplayResourceMenu();

	// Displays & allows modification of collision marks.
	// & calls the related functions.
	void DisplayCollisionMenu();

	// Displays the focused EditorElement's info.
	// & calls the related functions.
	void DisplayInspectorMenu();

	// Viewport size is changed by dragging.
	bool SceneViewportSizeChanged();

	// Does Foxtrot Project exists in projDir?
	bool ProjectExists(std::string projDir);

	void DisplayInfoMessage();
	void PopUpInfo(const char* msg);

	void DisplayErrorMessage();
	void PopUpError(const char* title, const char* msg);

	// Called according to FileMenuEvents.
	void CreateNewProject	(std::filesystem::path& path);
	void OpenProject		(std::filesystem::path& path);
	void Save				(std::filesystem::path& path);
	void SaveAs				(std::filesystem::path& path);
	void Open				(std::filesystem::path& path);
};