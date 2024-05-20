#ifdef _DEBUG
#pragma once
#include "SingletonMacro.h"
#include "EditorElement.h"
#include <functional>
#include <nlohmann/json.hpp>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>

class FoxtrotRenderer;
class UIActor;
class PanelUI;
class Command;
class CommandHistory;

#define VALUEMOD_SPEED 0.01f

class EditorLayer
{
	SINGLETON(EditorLayer);

public:
	void Update(float deltaTime);
	void Render(FoxtrotRenderer* renderer);
	void ShutDown();

public:
	std::vector<EditorElement*>& GetEditorElements() { return mEditorElements; }
	void AddEditorElement(Scene* scene);
	void AddEditorElement(Actor* ele);
	void DisplayEditorElements(FoxtrotRenderer* renderer);
	int& GetActorNameIdx() { return mActorNameIdx; }
	bool GetUndoKeyPressed()	const { return mUndoKeyPressed; }
	bool GetRedoKeyPressed()	const { return mRedoKeyPressed; }
	bool GetConfirmKeyPressed() const { return mConfirmKeyPressed; }
	ImVec2 GetSceneViewportPos() const { return mSceneViewportPos; }
	ImVec2 GetSceneViewportSize() const { return mSceneViewportSize; }

private:
	// After directX implementation
	// ImVec2 mViewportSize;
	std::vector<EditorElement*> mEditorElements;
	bool mCurrFileSaved;
	std::string mCurrFilePathName;
	
	int mHierarchyIdx;
	int mActorNameIdx;

	bool mSaveKeyPressed;
	bool mSaveAsKeyPressed;
	bool mOpenKeyPressed;
	bool mConfirmKeyPressed;

	bool mUndoKeyPressed;
	bool mRedoKeyPressed;
	bool mDeleteKeyPressed;

	ImVec2 mSceneViewportPos;
	ImVec2 mSceneViewportSize;
	bool mIsResizingViewport;

private:
	void DeleteAll();
	void UnfocusEditorElements();
	// After directX implementation
	void DisplayViewport();
	void DisplayFileMenu();
	void DisplayHierarchyMenu();
	void DisplayInspectorMenu();
	void ApplyCommandHistory();
	bool SceneViewportSizeChanged();
	//void ResizeUIWindow(std::string menuID);
};
#endif // _DEBUG