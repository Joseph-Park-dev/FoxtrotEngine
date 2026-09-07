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
		: mPlugins(DBG_NEW Common::FTDS::HashMap<Common::IPlugin*>(16))
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
        mOrder.clear();
    }
    void PluginManager::Attach(const char* name, Common::IPlugin* plugin)
    {
        if (GetPlugin(name)) return;
        mPlugins->Insert(name, plugin);
        mOrder.push_back(plugin);
    }
    Common::IPlugin* PluginManager::RegisterPlugin(const char* name)
    {
        // Compatibility lookup only. The EXE is the sole DLL loader.
        return GetPlugin(name);
    }

	Common::IPlugin* PluginManager::GetPlugin(const char* pluginName)
	{
		if (!mPlugins) return nullptr;
        for (auto it = mPlugins->Begin(); it != mPlugins->End(); ++it)
            if (*it && (*it)->Key().Equal(pluginName)) return (*it)->Value();
        return nullptr;
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
		for (auto plugin : mOrder) plugin->ProcessInput();
	}

	void PluginManager::Update(float deltaTime)
	{
		if (!mPlugins)
			return;
		for (auto plugin : mOrder) plugin->Update(deltaTime);
	}

	void PluginManager::LateUpdate(float deltaTime)
	{
		if (!mPlugins)
			return;
		for (auto plugin : mOrder) plugin->LateUpdate(deltaTime);
	}

	void PluginManager::Render()
	{
		if (!mPlugins)
			return;
		for (auto plugin : mOrder) plugin->Render();
	}

	void PluginManager::ProcessEvent()
	{
		if (!mPlugins)
			return;
		for (auto plugin : mOrder) plugin->ProcessEvent();
	}
} // namespace Core

extern "C"
{
	CORE_API Common::IPlugin* RegisterPlugin(const char* pluginName)
	{
		return Core::PluginManager::GetInstance()->RegisterPlugin(pluginName);
	}
}