#include "Manager/PluginManager.h"

#include <Windows.h>
#include <algorithm>

#include "Manager/Timer.h"
#include "FileSystem/FileTypes.h"

namespace
{
	template <typename Fn>
	void ForEachPlugin(std::vector<Common::IPlugin*>& order, Fn&& fn)
	{
		const std::vector<Common::IPlugin*> snapshot = order;
		for (Common::IPlugin* plugin : snapshot)
		{
			if (!plugin)
				continue;
			if (std::find(order.begin(), order.end(), plugin) == order.end())
				continue;
			fn(plugin);
		}
	}

	Core::PluginReloader gPluginReloader = nullptr;
	Core::PluginReloader gPluginUnloader = nullptr;
}

namespace Core
{
	Common::FTDS::HashMap<Common::IPlugin*>* GetAvailablePlugins()
	{
		return PluginManager::GetInstance()->GetPlugins();
	}

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

	/// @brief Removes a plugin from the registry without deleting it.
	/// @param name Name used when the plugin was attached.
	void PluginManager::Detach(const char* name)
	{
		if (!mPlugins || !name)
			return;
		Common::IPlugin* plugin = GetPlugin(name);
		if (!plugin)
			return;
		mPlugins->Erase(name);
		mOrder.erase(std::remove(mOrder.begin(), mOrder.end(), plugin), mOrder.end());
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

	void PluginManager::Initialize()
	{
		if (!mPlugins)
			return;
		ForEachPlugin(mOrder, [](Common::IPlugin* plugin) { plugin->Initialize(); });
	}

	/// @brief Calls Initialize on every component registered to each attached plugin, in attach order.
	void PluginManager::InitializeComponents()
	{
		if (!mPlugins)
			return;
		ForEachPlugin(mOrder, [](Common::IPlugin* plugin) { plugin->InitializeComponents(); });
	}

	void PluginManager::Setup()
	{
		if (!mPlugins)
			return;
		ForEachPlugin(mOrder, [](Common::IPlugin* plugin) { plugin->Setup(); });
	}

	/// @brief Dispatches input for the current frame to the relevant engine objects.
	void PluginManager::ProcessInput()
	{
		if (!mPlugins)
			return;
		ForEachPlugin(mOrder, [](Common::IPlugin* plugin) { plugin->ProcessInput(); });
	}

	/// @brief Advances frame-dependent state using the current time step.
	/// @param deltaTime Elapsed frame time in seconds.
	void PluginManager::Update(float deltaTime)
	{
		if (!mPlugins)
			return;
		ForEachPlugin(mOrder, [deltaTime](Common::IPlugin* plugin) { plugin->Update(deltaTime); });
	}

	/// @brief Runs the post-update lifecycle phase after ordinary frame updates.
	/// @param deltaTime Elapsed frame time in seconds.
	void PluginManager::LateUpdate(float deltaTime)
	{
		if (!mPlugins)
			return;
		ForEachPlugin(mOrder, [deltaTime](Common::IPlugin* plugin) { plugin->LateUpdate(deltaTime); });
	}

	/// @brief Submits this object's graphics work for the current frame.
	void PluginManager::Render()
	{
		if (!mPlugins)
			return;
		ForEachPlugin(mOrder, [](Common::IPlugin* plugin) { plugin->Render(); });
	}

	/// @brief Processes queued lifecycle and scene changes at the frame boundary.
	void PluginManager::ProcessEvent()
	{
		if (!mPlugins)
			return;
		ForEachPlugin(mOrder, [](Common::IPlugin* plugin) { plugin->ProcessEvent(); });
	}

	extern "C" void FT_CALL SetPluginReloader(PluginReloader reloader) noexcept
	{
		gPluginReloader = reloader;
	}

	extern "C" Foxtrot::Status FT_CALL ReloadPluginModule(const char* moduleName) noexcept
	{
		if (!moduleName)
			return Foxtrot::Status::InvalidArgument;
		if (!gPluginReloader)
			return Foxtrot::Status::Failed;
		return gPluginReloader(moduleName);
	}

	extern "C" void FT_CALL SetPluginUnloader(PluginReloader unloader) noexcept
	{
		gPluginUnloader = unloader;
	}

	extern "C" Foxtrot::Status FT_CALL UnloadPluginModule(const char* moduleName) noexcept
	{
		if (!moduleName)
			return Foxtrot::Status::InvalidArgument;
		if (!gPluginUnloader)
			return Foxtrot::Status::Failed;
		return gPluginUnloader(moduleName);
	}
} // namespace Core

//extern "C"
//{
//	/// @brief Obtains a plugin instance and registers it for engine lifecycle dispatch.
//	/// @param pluginName Plugin's registered name.
//	/// @return Obtains a plugin instance and registers it for engine lifecycle dispatch.
//	CORE_API Common::IPlugin* RegisterPlugin(const char* pluginName)
//	{
//		return Core::PluginManager::GetInstance()->GetPlugin(pluginName);
//	}
//}
