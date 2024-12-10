#pragma once
#include "Core/SingletonMacro.h"

class UIComponent;

class UIManager
{
	SINGLETON(UIManager);

public:
	void RegisterUI(UIComponent* UI);
	void Update(float deltaTime);
	//void SetFocusedUI(UIComponent* UI);
	void Reset();

private:
	UIComponent* GetFocusedUI();
	// Returns Overlapped targeted UI within parentUI's area. 
	UIComponent* GetTargetedUI(UIComponent* parentUI);

private:
	UIComponent* mFocusedUI;
	std::vector<UIComponent*> mUIs;

#ifdef FOXTROT_EDITOR
//public:
//	void EditorUpdate(float deltaTime);
//private:
//	UIComponent* EditorGetFocusedUI();
#endif // FOXTROT_EDITOR
};

