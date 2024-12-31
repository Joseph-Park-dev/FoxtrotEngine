// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Displays editor window, Provides functionality to create & edit .chunk files.
/// This also runs the EditorScene, emulating the actual Scene in produced game.
/// </summary>

#pragma once
#include "Core/FTCore.h"

#include "Debugging/DebugMemAlloc.h"

class FTCoreEditor :
    public FTCore
{
	SINGLETON(FTCoreEditor);

public:
	virtual bool Initialize() override;
	virtual void ShutDown()	  override;

public:
	LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) override;

public:
	bool GetIsUpdatingGame() const { return mIsUpdatingGame; }
	void SetIsUpdatingGame(bool isUpdating) { mIsUpdatingGame = isUpdating; }

private:
	bool			 mIsUpdatingGame;
	bool			 mIsResizingWindow;
	std::string		 mEditorDataFileName;

private:
	// These member functions are to be included in Initialize()
	void InitSingletonManagers() override;
	bool InitGUI();

private:
	// GameLoop functions
	void ProcessInput()   override;
	void UpdateGame()	  override;
	void GenerateOutput() override;
	void ProcessEvent()	  override;
};