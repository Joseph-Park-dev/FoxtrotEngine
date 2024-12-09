#pragma once

/// <FTCore_Game>
/// Game fields & functions for development -> Active in Release build; 
/// Loads .chunk files, runs the actual game.
/// </FTCore_Editor>

#pragma comment(lib,"d3d11.lib")
#include <vector>
#include <string>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "Core/SingletonMacro.h"
#include "Math/FTMath.h"

class FoxtrotRenderer;

class FTCore
{
	SINGLETON_PROTECTED(FTCore)

public:
	virtual bool Initialize();
			void RunLoop();
	virtual void ShutDown();

	// Constantly called in win32 MsgProc() function.
	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:
	// Initializes Common functionality.
	virtual bool InitializeWindow();
			bool InitFoxtrotRenderer_D3D11();
			void LoadGameData();
			void InitTimer();

protected:
	virtual	void InitSingletonManagers();

protected:
	// Gameloop functions.
	virtual void ProcessInput();
	virtual void UpdateGame();
	virtual void GenerateOutput();
	virtual void ProcessEvent();

public:
	HWND&			 GetWindow()			 { return mWindow; }
	FoxtrotRenderer* GetGameRenderer()		 { return mGameRenderer; }
	int				 GetWindowWidth()  const { return mWindowWidth; }
	int				 GetWindowHeight() const { return mWindowHeight; }
	std::wstring	 GetWindowTitle()		 { return mWindowTitle; }

	void SetWindowWidth  (int width)		  { mWindowWidth = width; }
	void SetWindowHeight (int height)		  { mWindowHeight = height; }
	void SetWindowTitle	 (std::wstring title) { mWindowTitle = title; }
	void SetIsRunning	 (bool isRunning)	  { mIsRunning = isRunning; }

private:
	HWND			 mWindow;
	FoxtrotRenderer* mGameRenderer;
	bool			 mIsRunning;
	int				 mWindowWidth;
	int				 mWindowHeight;
	std::wstring	 mWindowTitle;

private:
	std::string		 mGameDataPath;
};