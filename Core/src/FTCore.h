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
#include "FTDS/Static/FTString.h"
#include "FTDS/Static/HashMap.h"
#include "Plugin/CoreExports.h"

namespace Core
{
	class FTWindow;
	class FTInputDevice;
	class FoxtrotRenderer;
	class Plugin;
	class Entity;

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
		FTDS::HashMap<Entity*>* GetEntities() { return mEntities; }
		FTDS::HashMap<Plugin*>* GetPlugins() { return mPlugins; }
		Plugin*					GetPlugin(FTDS::String& pluginName);

		virtual void SetIsRunning(bool isRunning)
		{
			mIsRunning = isRunning;
		}
		void	SetIsUpdating(bool isUpdating) { mIsUpdating = isUpdating; }
		Plugin* RegisterPlugin(HMODULE mod, FTDS::String& pluginName);

	protected:
		// Gameloop functions.
		virtual void ProcessInput();
		virtual void UpdateGame();
		virtual void GenerateOutput();
		virtual void ProcessEvent();

	protected:
		virtual void LoadGameData();

	private:
		HMODULE mModule;
		bool	mIsRunning;
		bool	mIsUpdating;

	private:
		FTDS::String*			mGameDataPath;
		FTDS::HashMap<Entity*>* mEntities;
		FTDS::HashMap<Plugin*>* mPlugins;

	private:
		void		   LoadDLL(FTDS::String& path);
		void		   InitTimer();
		void		   InitEntities();
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
} // namespace Core