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
	ChunkIsSaved
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

	void SetErrorType(ErrorType type) { mErrorType = type; }

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

	void DisplayInspectorMenu();
	bool SceneViewportSizeChanged();
	bool ProjectExists(std::string projDir);

	void DisplayInfoMessage();
	void PopUpInfo_ChunkIsSaved();

	void DisplayErrorMessage();
	void PopUpError_ProjectPathExists();
	void PopUpError_ProjectNotValid();
	void PopUpError_FolderNotEmpty();
	void PopUpError_ChunkNotSaved();

	void CreateNewProject(std::filesystem::path& path);
	void OpenProject(std::filesystem::path& path);
	void Save(std::filesystem::path& path);
	void SaveAs(std::filesystem::path& path);
	void Open(std::filesystem::path& path);
};