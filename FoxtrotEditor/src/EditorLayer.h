#pragma once
#include <functional>
#include <nlohmann/json.hpp>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>

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
	ChunkNotSaved,
	ProjectNotValid
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
	int& GetActorNameIdx() { return mActorNameIdx; }
	bool GetUndoKeyPressed()	const { return mUndoKeyPressed; }
	bool GetRedoKeyPressed()	const { return mRedoKeyPressed; }
	bool GetConfirmKeyPressed() const { return mConfirmKeyPressed; }
	ImVec2 GetSceneViewportPos() const { return mSceneViewportPos; }
	ImVec2 GetSceneViewportSize() const { return mSceneViewportSize; }
	bool GetCurrentFileSaved() const { return mCurrFileSaved; }
	ErrorType GetErrorType() { return mErrorType; }

	void SetErrorType(ErrorType type) { mErrorType = type; }

private:
	// After directX implementation
	// ImVec2 mViewportSize;
	UINT		mElementNumber;
	bool		mCurrFileSaved;
	std::string mCurrProjectPath;
	std::string mCurrFilePath;
	
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
	ErrorType		mErrorType;

private:
	// After directX implementation
	void DisplayViewport();
	void DisplayFileMenu();
	void DisplayHierarchyMenu();
	void DisplayResourceMenu();

	void DisplayInspectorMenu();
	bool SceneViewportSizeChanged();
	bool ProjectPathExists(std::string& projDir);

	void SaveChunkFromUI();

	void DisplayInfoMessage();
	void PopUpInfo_ChunkIsSaved();

	void DisplayErrorMessage();
	void PopUpError_ProjectPathExists();
	void PopUpError_ProjectNotValid();
	void PopUpError_ChunkNotSaved();
};