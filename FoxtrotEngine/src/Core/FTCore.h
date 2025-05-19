// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// The core class that runs the produced game.
/// Initializes a game instance, runs the game loop, and does shutdown
/// operation when terminating the game instance.
/// </summary>

#pragma once
#pragma comment(lib, "d3d11.lib")
#include <vector>
#include <string>
#define WIN32_LEAN_AND_MEAN

#include "Core/SingletonMacro.h"
#include "Math/FTMath.h"
#include "Static/FTString.h"

class FTWindow;
class FoxtrotRenderer;

class FTCore
{
	SINGLETON_PROTECTED(FTCore)

public:
	virtual bool Initialize();
	void		 RunLoop();
	virtual void ShutDown();

	// Constantly called in win32 MsgProc() function.
	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

public:
	FTWindow*		 GetGameWindow() { return mWindow; }
	FoxtrotRenderer* GetGameRenderer() { return mGameRenderer; }

	void SetIsRunning(bool isRunning) { mIsRunning = isRunning; }

protected:
	// Gameloop functions.
	virtual void ProcessInput();
	virtual void UpdateGame();
	virtual void GenerateOutput();
	virtual void ProcessEvent();

protected:
	virtual bool InitFoxtrotRenderer_D3D11(FTWindow* window, int& width, int& height);
	virtual void InitSingletonManagers();
	virtual void LoadGameData();

private:
	FTWindow*		 mWindow;
	FoxtrotRenderer* mGameRenderer;
	bool			 mIsRunning;

	FTDS::String mWindowTitle;
	int			 mWindowWidth;
	int			 mWindowHeight;

private:
	FTDS::String mGameDataPath;

private:
	// Initializes Common functionality.
	void InitTimer();
};