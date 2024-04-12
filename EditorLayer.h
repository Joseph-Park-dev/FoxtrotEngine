#ifdef _DEBUG
#pragma once
#include <functional>
#include <nlohmann/json.hpp>

#include "SingletonMacro.h"
#include "EditorElement.h"

struct SDL_Window;
struct SDL_Renderer;
union SDL_Event;
class UIActor;
class PanelUI;
class Command;
class CommandHistory;

class EditorLayer
{
	SINGLETON(EditorLayer);

public:
	void Init(SDL_Window* window, SDL_Renderer* renderer);
	void ProcessInput(SDL_Event* event);
	void Update(float deltaTime);
	void Render(SDL_Renderer* renderer);
	void ShutDown();

public:
	std::vector<EditorElement*>& GetEditorElements() { return mEditorElements; }
	void AddEditorElement(Actor* actor);
	void DisplayEditorElements(SDL_Renderer* renderer);

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
	void UnfocusEditorElements();
	// After directX implementation
	// void DisplayViewport();
	void DisplayFileMenu();
	void DisplayHierarchyMenu();
	void ApplyCommandHistory();
};
#endif // _DEBUG