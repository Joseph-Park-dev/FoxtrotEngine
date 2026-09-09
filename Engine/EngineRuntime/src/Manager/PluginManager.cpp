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

	/// @brief Initializes the registry used for plugin lifecycle dispatch.
	/// @note Initializes the :PluginManager base or delegates to its constructor.
	PluginManager::PluginManager()
		: mPlugins(DBG_NEW Common::FTDS::HashMap<Common::IPlugin*>(16))
	{
	}

	/// @brief Releases the resources managed by this instance during destruction.
	PluginManager::~PluginManager()
	{
		ShutDown();
	}

	/// @brief Releases runtime services and resources during engine shutdown.
	void PluginManager::ShutDown()
	{
		if (mPlugins)
		{
			Safe_Delete_Map(mPlugins);
			mPlugins = nullptr;
		}
		mOrder.clear();
	}

	/// @brief Associates the supplied implementation with the owning host or registry.
	/// @param name Name used to identify the requested object or interface.
	/// @param plugin Plugin supplying component or lifecycle services.
	void PluginManager::Attach(const char* name, Common::IPlugin* plugin)
	{
		if (GetPlugin(name))
			return;
		mPlugins->Insert(name, plugin);
		mOrder.push_back(plugin);
	}

	/// @brief Obtains a plugin instance and registers it for engine lifecycle dispatch.
	/// @param name Name used to identify the requested object or interface.
	/// @return Obtains a plugin instance and registers it for engine lifecycle dispatch.
	Common::IPlugin* PluginManager::RegisterPlugin(const char* name)
	{
		// Compatibility lookup only. The EXE is the sole DLL loader.
		return GetPlugin(name);
	}

	/// @brief Returns the plugin used by this plugin manager.
	/// @param pluginName Plugin's registered name.
	/// @return Borrowed access to the plugin. May be null when no matching object is available.
	Common::IPlugin* PluginManager::GetPlugin(const char* pluginName)
	{
		if (!mPlugins)
			return nullptr;
		for (auto it = mPlugins->Begin(); it != mPlugins->End(); ++it)
			if (*it && (*it)->Key().Equal(pluginName))
				return (*it)->Value();
		return nullptr;
	}

	/// @brief Returns the plugins used by this plugin manager.
	/// @return Borrowed access to the plugins.
	Common::FTDS::HashMap<Common::IPlugin*>* PluginManager::GetPlugins()
	{
		return mPlugins;
	}

	/// @brief Provides an empty lifecycle or extension hook for this implementation.
	/// @param actor Actor participating in this operation.
	void PluginManager::LoadComponents(Common::IActor* actor)
	{
	}

	/// @brief Dispatches input for the current frame to the relevant engine objects.
	void PluginManager::ProcessInput()
	{
		if (!mPlugins)
			return;
		for (auto plugin : mOrder)
			plugin->ProcessInput();
	}

	/// @brief Advances frame-dependent state using the current time step.
	/// @param deltaTime Elapsed frame time in seconds.
	void PluginManager::Update(float deltaTime)
	{
		if (!mPlugins)
			return;
		for (auto plugin : mOrder)
			plugin->Update(deltaTime);
	}

	/// @brief Runs the post-update lifecycle phase after ordinary frame updates.
	/// @param deltaTime Elapsed frame time in seconds.
	void PluginManager::LateUpdate(float deltaTime)
	{
		if (!mPlugins)
			return;
		for (auto plugin : mOrder)
			plugin->LateUpdate(deltaTime);
	}

	/// @brief Submits this object's graphics work for the current frame.
	void PluginManager::Render()
	{
		if (!mPlugins)
			return;
		for (auto plugin : mOrder)
			plugin->Render();
	}

	/// @brief Processes queued lifecycle and scene changes at the frame boundary.
	void PluginManager::ProcessEvent()
	{
		if (!mPlugins)
			return;
		for (auto plugin : mOrder)
			plugin->ProcessEvent();
	}
} // namespace Core

extern "C"
{
	/// @brief Obtains a plugin instance and registers it for engine lifecycle dispatch.
	/// @param pluginName Plugin's registered name.
	/// @return Obtains a plugin instance and registers it for engine lifecycle dispatch.
	CORE_API Common::IPlugin* RegisterPlugin(const char* pluginName)
	{
		return Core::PluginManager::GetInstance()->RegisterPlugin(pluginName);
	}
}
