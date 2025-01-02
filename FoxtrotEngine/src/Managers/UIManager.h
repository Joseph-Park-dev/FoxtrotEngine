// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// A singleton manager that takes care of the UIComponents registered
/// and executes the basic UI operations such as mouse clicks.
/// </summary>

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
};