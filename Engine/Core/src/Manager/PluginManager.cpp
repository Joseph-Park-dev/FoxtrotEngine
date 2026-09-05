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

	void PluginManager::ShutDown()
	{
		Safe_Delete_Map(mPlugins);
		for (auto iter = mModules->Begin(); iter != mModules->End(); ++iter)
			FreeLibrary((*iter)->Value());
		delete mModules;
	}

	Common::IPlugin* PluginManager::RegisterPlugin(const char* pluginName)
	{
		Common::FTDS::String path = pluginName;
		path.Append(Common::FileTypes::DLL);

		HMODULE mod				  = LoadLibraryA(path.C_Str());
		FARPROC proc			  = GetProcAddress(mod, Core::PluginKey::CREATE_PLUGIN);
		using PLUGIN_CONSTRUCT	  = Common::IPlugin* (*)();
		PLUGIN_CONSTRUCT plgConst = reinterpret_cast<PLUGIN_CONSTRUCT>(proc);
		Common::IPlugin*	 plugin	  = plgConst();

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