#pragma once
#include "Utility/SingletonMacro.h"
#include "FTDS/Static/HashMap.h"
#include "Plugin/IPlugin.h"

#include "Plugin/PluginKey.h"


namespace Main
{
class Engine
{
public:
	/// @brief Returns the shared instance used by this manager.
	/// @return Borrowed singleton pointer; do not delete it directly.
	/// @note Synchronize concurrent initialization, access, and destruction externally.
	static Engine* GetInstance()
	{
		if (mInstance == nullptr)
			mInstance = DBG_NEW Engine();
		return mInstance;
	}
	/// @brief Deletes the shared instance and clears the singleton pointer.
	/// @note Invalidates borrowed pointers to the instance; synchronize concurrent access externally.
	static void Destroy()
	{
		if (mInstance)
		{
			delete mInstance;
			mInstance = nullptr;
		}
	}
	/// @brief Disables copying so the instance's managed state cannot be duplicated.
	/// @param obj Source object; copying is unavailable when this overload is deleted.
	Engine(const Engine& obj) = delete;

public:
	/// @brief Runs input, update, rendering, and event processing until the engine stops.
	void RunLoop();
	/// @brief Releases runtime services and resources during engine shutdown.
	void ShutDown();

public:
	/// @brief Obtains a plugin instance and registers it for engine lifecycle dispatch.
	/// @param mod Loaded module containing the plugin entry point.
	/// @param pluginName Plugin's registered name.
	/// @return Obtains a plugin instance and registers it for engine lifecycle dispatch.
	virtual Core::IPlugin* RegisterPlugin(HMODULE mod, const char* pluginName)
	{
		FARPROC proc			  = GetProcAddress(mod, Core::PluginKey::CREATE_PLUGIN);
		using PLUGIN_CONSTRUCT	  = Core::IPlugin* (*)();
		PLUGIN_CONSTRUCT plgConst = reinterpret_cast<PLUGIN_CONSTRUCT>(proc);
		Core::IPlugin*	 plugin	  = plgConst();
		plugin->Initialize();
		plugin->Setup();
		mPlugins->Insert(pluginName, plugin);

		return plugin;
	}

	/// @brief Returns the plugin used by this engine.
	/// @param pluginName Plugin's registered name.
	/// @return Borrowed access to the plugin.
	virtual Core::IPlugin* GetPlugin(const char* pluginName)
	{
		return mPlugins->At(pluginName)->Value();
	}

	/// @brief Returns the plugins used by this engine.
	/// @return Borrowed access to the plugins.
	Common::FTDS::HashMap<Core::IPlugin*>* GetPlugins()
	{
		return mPlugins;
	}

	/// @brief Updates the is running used by subsequent operations.
	/// @param val Replacement is running.
	void SetIsRunning(bool val) { mIsRunning = val; }
	/// @brief Updates the is updating used by subsequent operations.
	/// @param val Replacement is updating.
	void SetIsUpdating(bool val) { mIsUpdating = val; }

public:
	/// @brief Initializes the plugin registry and engine loop flags.
	Engine()
		: mPlugins(DBG_NEW Common::FTDS::HashMap<Core::IPlugin*>())
		, mIsRunning(true)
		, mIsUpdating(true)
	{
	}

private:
	inline static Engine* mInstance = nullptr;

private:
	Common::FTDS::HashMap<Core::IPlugin*>* mPlugins;

private:
	// Gameloop functions.
	/// @brief Dispatches input for the current frame to the relevant engine objects.
	void ProcessInput();
	/// @brief Advances the active plugins using the elapsed frame time.
	void UpdateGame();
	/// @brief Dispatches the rendering phase for the active plugins.
	void GenerateOutput();
	/// @brief Processes queued lifecycle and scene changes at the frame boundary.
	void ProcessEvent();

private:
	bool mIsRunning;
	bool mIsUpdating;
};
} // namespace Main
