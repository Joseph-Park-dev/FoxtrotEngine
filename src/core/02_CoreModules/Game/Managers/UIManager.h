#pragma once
#include "SingletonMacro.h"

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

	void	 EditorUpdate(float deltaTime);

private:
	UIActor* GetFocusedUI();
	// Returns Overlapped targeted UI within parentUI's area. 
	UIActor* GetTargetedUI(UIActor* parentUI);
	UIActor* EditorGetFocusedUI();
};

