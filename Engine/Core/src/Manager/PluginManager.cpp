#include "Manager/PluginManager.h"

#include <Windows.h>

#include "Manager/Timer.h"
#include "FileSystem/FileTypes.h"

namespace Core
{
	namespace GameData
	{
		constexpr const char* DLL_LIST = "DLL List";
	} // namespace GameData

	PluginManager::PluginManager()
		: mModules(DBG_NEW Common::FTDS::HashMap<HMODULE>)
		, mPlugins(DBG_NEW Common::FTDS::HashMap<Common::IPlugin*>)
	{
	}

	PluginManager::~PluginManager()
	{
		ShutDown();
	}

	void PluginManager::ShutDown()
	{
		if (mPlugins)
		{
			Safe_Delete_Map(mPlugins);
			mPlugins = nullptr;
		}
		if (mModules)
		{
			for (auto iter = mModules->Begin(); iter != mModules->End(); ++iter)
				FreeLibrary((*iter)->Value());
			delete mModules;
			mModules = nullptr;
		}
	}

	Common::IPlugin* PluginManager::RegisterPlugin(const char* pluginName)
	{
		Common::FTDS::String path = pluginName;
		path.Append(Common::FileTypes::DLL);

		HMODULE mod = LoadLibraryA(path.C_Str());
		if (!mod)
			return nullptr;

		FARPROC proc = GetProcAddress(mod, Core::PluginKey::CREATE_PLUGIN);
		if (!proc)
			return nullptr;

		using PLUGIN_CONSTRUCT	  = Common::IPlugin* (*)(const char*);
		PLUGIN_CONSTRUCT plgConst = reinterpret_cast<PLUGIN_CONSTRUCT>(proc);
		Common::IPlugin* plugin	  = plgConst(pluginName);
		if (!plugin)
			return nullptr;

		plugin->Initialize();
		plugin->Setup();

		mModules->Insert(pluginName, mod);
		mPlugins->Insert(pluginName, plugin);
		return plugin;
	}

	Common::IPlugin* PluginManager::GetPlugin(const char* pluginName)
	{
		return mPlugins->At(pluginName)->Value();
	}

	Common::FTDS::HashMap<Common::IPlugin*>* PluginManager::GetPlugins()
	{
		return mPlugins;
	}

	void PluginManager::LoadComponents(Common::IActor* actor)
	{
	}

	void PluginManager::ProcessInput()
	{
		if (!mPlugins)
			return;
		for (auto iter = mPlugins->Begin(); iter != mPlugins->End(); ++iter)
			(*iter)->Value()->ProcessInput();
	}

	void PluginManager::Update(float deltaTime)
	{
		if (!mPlugins)
			return;
		for (auto iter = mPlugins->Begin(); iter != mPlugins->End(); ++iter)
			(*iter)->Value()->Update(deltaTime);
	}

	void PluginManager::LateUpdate(float deltaTime)
	{
		if (!mPlugins)
			return;
		for (auto iter = mPlugins->Begin(); iter != mPlugins->End(); ++iter)
			(*iter)->Value()->LateUpdate(deltaTime);
	}

	void PluginManager::Render()
	{
		if (!mPlugins)
			return;
		for (auto iter = mPlugins->Begin(); iter != mPlugins->End(); ++iter)
			(*iter)->Value()->Render();
	}

	void PluginManager::ProcessEvent()
	{
		if (!mPlugins)
			return;
		for (auto iter = mPlugins->Begin(); iter != mPlugins->End(); ++iter)
			(*iter)->Value()->ProcessEvent();
	}
} // namespace Core

extern "C"
{
	CORE_API Common::IPlugin* RegisterPlugin(const char* pluginName)
	{
		return Core::PluginManager::GetInstance()->RegisterPlugin(pluginName);
	}
}