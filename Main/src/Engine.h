#pragma once
#include "SingletonMacro.h"
#include "FTDS/Static/HashMap.h"
#include "Plugin/PluginKey.h"

namespace Core
{
	class Plugin;
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
	virtual Core::Plugin* RegisterPlugin(HMODULE mod, const char* pluginName)
	{
		FARPROC proc			  = GetProcAddress(mod, Core::PluginKey::CREATE_PLUGIN);
		using PLUGIN_CONSTRUCT	  = Core::Plugin* (*)(const char*);
		PLUGIN_CONSTRUCT plgConst = reinterpret_cast<PLUGIN_CONSTRUCT>(proc);
		Core::Plugin*	 plugin	  = plgConst(pluginName);
		plugin->SetModule(mod);
		plugin->Initialize();
		plugin->Setup();
		mPlugins->Insert(pluginName, plugin);

		return plugin;
	}

	virtual Core::Plugin* GetPlugin(const char* pluginName)
	{
		return mPlugins->At(pluginName)->Value();
	}

public:
	Engine()
		: mPlugins(DBG_NEW Core::FTDS::HashMap<Core::Plugin*>())
	{
	}

private:
	Core::FTDS::HashMap<Core::Plugin*>* mPlugins;

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
