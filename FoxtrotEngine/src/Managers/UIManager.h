#pragma once
#include "Core/SingletonMacro.h"

class UIActor;

class UIManager
{
	SINGLETON(UIManager);

private:
	UIActor*	mFocusedUI;

public:
	void	 Update(float deltaTime);
	void	 SetFocusedUI(UIActor* UI);
	void	 Reset();

private:
	UIActor* GetFocusedUI();
	// Returns Overlapped targeted UI within parentUI's area. 
	UIActor* GetTargetedUI(UIActor* parentUI);

#ifdef FOXTROT_EDITOR
	void EditorUpdate(float deltaTime);
	UIActor* EditorGetFocusedUI();
#endif // FOXTROT_EDITOR
};

