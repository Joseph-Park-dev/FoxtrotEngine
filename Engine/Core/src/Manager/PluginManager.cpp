#include "Manager/PluginManager.h"

#include <Windows.h>

#include "Manager/Timer.h"
#include "FileSystem/FileTypes.h"

namespace GameData
{
	constexpr const char* DLL_LIST = "DLL List";
} // namespace GameData

namespace Core
{
	PluginManager::PluginManager()
		: mModules(DBG_NEW Common::FTDS::HashMap<HMODULE>)
		, mPlugins(DBG_NEW Common::FTDS::HashMap<Core::IPlugin*>)
	{
	}

	void PluginManager::ShutDown()
	{
		Safe_Delete_Map(mPlugins);
		for (auto iter = mModules->Begin(); iter != mModules->End(); ++iter)
			FreeLibrary((*iter)->Value());
		delete mModules;
	}

	Core::IPlugin* PluginManager::RegisterPlugin(const char* pluginName)
	{
		Common::FTDS::String path = pluginName;
		path.Append(Core::FileTypes::DLL);

		HMODULE mod				  = LoadLibraryA(path.C_Str());
		FARPROC proc			  = GetProcAddress(mod, Core::PluginKey::CREATE_PLUGIN);
		using PLUGIN_CONSTRUCT	  = Core::IPlugin* (*)();
		PLUGIN_CONSTRUCT plgConst = reinterpret_cast<PLUGIN_CONSTRUCT>(proc);
		Core::IPlugin*	 plugin	  = plgConst();

		mModules->Insert(pluginName, mod);
		mPlugins->Insert(pluginName, plugin);
		return plugin;
	}

	Core::IPlugin* PluginManager::GetPlugin(const char* pluginName)
	{
		return mPlugins->At(pluginName)->Value();
	}

	Common::FTDS::HashMap<Core::IPlugin*>* PluginManager::GetPlugins()
	{
		return mPlugins;
	}

	void PluginManager::ProcessInput()
	{
		for (auto iter = mPlugins->Begin(); iter != mPlugins->End(); ++iter)
			(*iter)->Value()->ProcessInput();
	}

	void PluginManager::Update(float deltaTime)
	{
		for (auto iter = mPlugins->Begin(); iter != mPlugins->End(); ++iter)
			(*iter)->Value()->Update(deltaTime);
	}

	void PluginManager::LateUpdate(float deltaTime)
	{
		for (auto iter = mPlugins->Begin(); iter != mPlugins->End(); ++iter)
			(*iter)->Value()->LateUpdate(deltaTime);
	}

	void PluginManager::Render()
	{
		for (auto iter = mPlugins->Begin(); iter != mPlugins->End(); ++iter)
			(*iter)->Value()->Render();
	}

	void PluginManager::ProcessEvent()
	{
		for (auto iter = mPlugins->Begin(); iter != mPlugins->End(); ++iter)
			(*iter)->Value()->ProcessEvent();
	}
} // namespace Core