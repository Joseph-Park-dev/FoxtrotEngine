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

#include "FTDS/Static/FTString.h"

#ifdef CORE_EXPORTS
	#define CORE_API __declspec(dllexport)
#else
	#define CORE_API __declspec(dllimport)
#endif

class FTWindow;
class FTInputDevice;
class FoxtrotRenderer;
class Plugin;
namespace FTDS
{
	template <typename TYPE>
	class HashMap;
}

class CORE_API FTCore
{
	SINGLETON_PROTECTED(FTCore)

public:
	virtual bool Initialize();
	void		 RunLoop();
	virtual void ShutDown();

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
	virtual void InitSingletonManagers();
	virtual void LoadGameData();

private:
	FTWindow*		 mWindow;
	FTInputDevice*	 mInputDevice;
	FoxtrotRenderer* mGameRenderer;
	bool			 mIsRunning;

private:
	FTDS::String*			mGameDataPath;
	FTDS::HashMap<Plugin*>* mLoadedPlugins;

private:
	void LoadDLL(FTDS::String& path);
	void InitTimer();
};

extern "C"
{
	CORE_API void Create_Core();
}

namespace GameData
{
	constexpr const char* TITLE		 = "Game Data";
	constexpr const char* CHUNK_LIST = "Chunk List";
	constexpr const char* DLL_LIST	 = "DLL List";
} // namespace GameData

namespace PluginKey
{
	namespace FTCore
	{
		constexpr const char* CREATE = "Create_Core";
	}
} // namespace Plugin