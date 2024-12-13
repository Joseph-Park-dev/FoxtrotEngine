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

enum class FileMenuEvents
{
	None,
	NewProject,
	OpenProject,
	Save,
	SaveAs,
	Open
};

enum class InfoType
{
	None,
	ChunkIsSaved,
	PremadeIsCreated
};

class EditorLayer
{
	SINGLETON(EditorLayer);

public:
	void Update(float deltaTime);
	void Render(FoxtrotRenderer* renderer);
	void ShutDown();

public:
	int&		GetActorNameIdx() { return mActorNameIdx; }
	bool		GetUndoKeyPressed()	const { return mUndoKeyPressed; }
	bool		GetRedoKeyPressed()	const { return mRedoKeyPressed; }
	bool		GetConfirmKeyPressed() const { return mConfirmKeyPressed; }
	ImVec2		GetSceneViewportPos() const { return mSceneViewportPos; }
	ImVec2		GetSceneViewportSize() const { return mSceneViewportSize; }
	ErrorType	GetErrorType() { return mErrorType; }

	void		SetInfoType(InfoType type) { mInfoType = type; }
	void		SetErrorType(ErrorType type) { mErrorType = type; }

public:
	template <class FUNCTOR>
	void PopUpInquiry(const char* title, const char* msg, FUNCTOR&& onConfirm)
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

	template <class FUNCTOR>
	void PopUpInquiry(const char* title, const char* msg, FUNCTOR onConfirm, FUNCTOR&& onCancel)
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

			ImGui::CloseCurrentPopup();
			mInfoType = InfoType::None;
			ImGui::EndPopup();
		}
	}

private:
	// After directX implementation
	// ImVec2 mViewportSize;
	UINT		mElementNumber;
	
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

	// UI event types
	InfoType		mInfoType;
	FileMenuEvents	mFileMenuEvent;
	ErrorType		mErrorType;

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
	void TEST_Instantiate();
	// After directX implementation
	void DisplayViewport();
	void DisplayFileMenu();
	void DisplayHierarchyMenu();
	void DisplayResourceMenu();
	void DisplayCollisionMenu();

	void DisplayInspectorMenu();
	bool SceneViewportSizeChanged();
	bool ProjectExists(std::string projDir);

	void DisplayInfoMessage();
	void PopUpInfo(const char* msg);

	void DisplayErrorMessage();
	void PopUpError(const char* title, const char* msg);

	void CreateNewProject(std::filesystem::path& path);
	void OpenProject(std::filesystem::path& path);
	void Save(std::filesystem::path& path);
	void SaveAs(std::filesystem::path& path);
	void Open(std::filesystem::path& path);
};