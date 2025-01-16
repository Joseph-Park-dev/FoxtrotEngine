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

class UI;

class UIManager
{
	SINGLETON(UIManager);

public:
	void RegisterUI(UI* UI);
	void Update(float deltaTime);
	//void SetFocusedUI(UI* UI);
	void Reset();

private:
	UI* GetFocusedUI();
	// Returns Overlapped targeted UI within parentUI's area. 
	UI* GetTargetedUI(UI* parentUI);

private:
	UI* mFocusedUI;
	std::vector<UI*> mUIs;
};