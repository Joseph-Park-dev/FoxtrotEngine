#ifdef _DEBUG
#pragma once
#include "SingletonMacro.h"
#include "EditorElement.h"
#include <functional>
#include <nlohmann/json.hpp>

struct SDL_Window;
struct FoxtrotRenderer;
union SDL_Event;
class UIActor;
class PanelUI;
class Command;
class CommandHistory;

class EditorLayer
{
	SINGLETON(EditorLayer);

public:
	void Update(float deltaTime);
	void Render(FoxtrotRenderer* renderer);
	void ShutDown();

public:
	std::vector<EditorElement*>& GetEditorElements() { return mEditorElements; }
	void AddEditorElement(Actor* actor);
	void DisplayEditorElements(FoxtrotRenderer* renderer);

	int& GetActorNameIdx() { return mActorNameIdx; }

	bool GetUndoKeyPressed()	const { return mUndoKeyPressed; }
	bool GetRedoKeyPressed()	const { return mRedoKeyPressed; }
	bool GetConfirmKeyPressed() const { return mConfirmKeyPressed; }

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

private:
	void Save();

private:
	void UnfocusEditorElements();
	// After directX implementation
	// void DisplayViewport();
	void DisplayFileMenu();
	void DisplayHierarchyMenu();
	void ApplyCommandHistory();
};
#endif // _DEBUG