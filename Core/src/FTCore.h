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
#include "SingletonMacro.h"
#include "Static/FTString.h"

#ifdef CORE_EXPORTS
	#define CORE_API __declspec(dllexport)
#else
	#define CORE_API __declspec(dllimport)
#endif

class FTWindow;
class FoxtrotRenderer;
class Plugin;

class CORE_API FTCore
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

private:
	FTDS::String			mGameDataPath;
	FTDS::HashMap<Plugin*>* mLoadedPlugins;

private:
	void LoadDLL(FTDS::String& path);
	void InitTimer();
};

extern "C"
{
	CORE_API FTCore* Create_Core();
}

namespace GameData
{
	constexpr const char* TITLE		 = "Game Data";
	constexpr const char* CHUNK_LIST = "Chunk List";
	constexpr const char* DLL_LIST	 = "DLL List";
} // namespace GameData

namespace Plugin
{
	namespace FTCore
	{
		constexpr const char* CREATE = "Create_Core";
	}
} // namespace Plugin