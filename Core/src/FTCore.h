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
#include "Plugin/CoreExports.h"

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
public:
	static FTCore* GetInstance()
	{
		if (mInstance == nullptr)
			mInstance = DBG_NEW FTCore();
		return mInstance;
	}
	static void Destroy()
	{
		if (mInstance)
		{
			delete mInstance;
			mInstance = nullptr;
		}
	}
	FTCore(const FTCore& obj) = delete;


protected:
	FTCore();
	~FTCore();

public:
	virtual bool Initialize();
	virtual void RunLoop();
	virtual void ShutDown();

public:
	FTWindow*		 GetGameWindow() { return mWindow; }
	FoxtrotRenderer* GetGameRenderer() { return mGameRenderer; }

	virtual void SetIsRunning(bool isRunning) { mIsRunning = isRunning; }
	virtual void SetWindow(FTWindow* window);
	virtual void SetInputDevice(FTInputDevice* device);
	virtual void SetRenderer(FoxtrotRenderer* renderer);

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
	FTDS::String*				 mGameDataPath;
	FTDS::DynamicArray<Plugin*>* mLoadedPlugins;

private:
	void LoadDLL(FTDS::String& path);
	void InitTimer();
	static FTCore* mInstance;
};

extern "C"
{
	CORE_API FTCore* GetInstanceCore();
	CORE_API void	 DestroyCore();
}

namespace GameData
{
	constexpr const char* TITLE		 = "Game Data";
	constexpr const char* CHUNK_LIST = "Chunk List";
	constexpr const char* DLL_LIST	 = "DLL List";
} // namespace GameData

namespace PluginKey
{
	constexpr const char* CREATE_PLUGIN = "CreatePlugin";
	namespace FTCore
	{
		constexpr const char* GET_INSTANCE = "GetInstanceCore";
		constexpr const char* DESTROY	   = "DestroyCore";
	} // namespace FTCore
} // namespace PluginKey

enum class PluginType
{
	D3D11,
	END
};

inline PluginType GetPluginType(FTDS::String& name)
{
	if (name.Equal("D3D11"))
		return PluginType::D3D11;
}