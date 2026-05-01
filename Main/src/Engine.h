#pragma once
#include "SingletonMacro.h"
#include "FTDS/Static/HashMap.h"
#include "Plugin/PluginKey.h"
#include "Plugin/IPlugin.h"

namespace Core
{
	class IPlugin;
}

class Engine
{
public:
	static Engine* GetInstance()
	{
		if (mInstance == nullptr)
			mInstance = DBG_NEW Engine();
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
	Engine(const Engine& obj) = delete;

private:
	inline static Engine* mInstance = nullptr;

public:
	void RunLoop();
	void ShutDown();

public:
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

	virtual Core::IPlugin* GetPlugin(const char* pluginName)
	{
		return mPlugins->At(pluginName)->Value();
	}

public:
	Engine()
		: mPlugins(DBG_NEW Core::FTDS::HashMap<Core::IPlugin*>())
	{
	}

private:
	Core::FTDS::HashMap<Core::IPlugin*>* mPlugins;

private:
	// Gameloop functions.
	void ProcessInput();
	void UpdateGame();
	void GenerateOutput();
	void ProcessEvent();

private:
	bool mIsRunning;
	bool mIsUpdating;
};
