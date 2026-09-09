#pragma once
#include <vector>
#include "Utility/SingletonMacro.h"

#include "FTDS/Static/HashMap.h"
#include "FTDS/Static/FTString.h"
#include "Plugin/IPlugin.h"
#include "Plugin/PluginKey.h"
#include "Plugin/CoreExports.h"

namespace Core
{
	constexpr const char* CREATE_PREFIX = "Create";

	class PluginManager
	{
		SINGLETON(PluginManager)


	public:
		// Gameloop functions.
		/// @brief Dispatches input for the current frame to the relevant engine objects.
		void ProcessInput();
		/// @brief Advances frame-dependent state using the current time step.
		/// @param deltaTime Elapsed frame time in seconds.
		void Update(float deltaTime);
		/// @brief Runs the post-update lifecycle phase after ordinary frame updates.
		/// @param deltaTime Elapsed frame time in seconds.
		void LateUpdate(float deltaTime);
		/// @brief Submits this object's graphics work for the current frame.
		void Render();
		/// @brief Processes queued lifecycle and scene changes at the frame boundary.
		void ProcessEvent();

		/// @brief Releases runtime services and resources during engine shutdown.
		void ShutDown();
        /// @brief Associates the supplied implementation with the owning host or registry.
        /// @param name Name used to identify the requested object or interface.
        /// @param plugin Plugin supplying component or lifecycle services.
        void Attach(const char* name, Common::IPlugin* plugin);

	public:
		/// @brief Obtains a plugin instance and registers it for engine lifecycle dispatch.
		/// @param pluginName Plugin's registered name.
		/// @return Obtains a plugin instance and registers it for engine lifecycle dispatch.
		Common::IPlugin* RegisterPlugin(const char* pluginName);
		/// @brief Returns the plugin used by this plugin manager.
		/// @param pluginName Plugin's registered name.
		/// @return Borrowed access to the plugin. May be null when no matching object is available.
		Common::IPlugin* GetPlugin(const char* pluginName);

		/// @brief Returns the plugins used by this plugin manager.
		/// @return Borrowed access to the plugins.
		Common::FTDS::HashMap<Common::IPlugin*>* GetPlugins();

	public:
		/// @brief Provides an empty lifecycle or extension hook for this implementation.
		/// @param actor Actor participating in this operation.
		void LoadComponents(Common::IActor* actor);

	private:
		std::vector<Common::IPlugin*> mOrder;
		Common::FTDS::HashMap<Common::IPlugin*>* mPlugins;
	};

	extern "C"
	{
		/// @brief Obtains a plugin instance and registers it for engine lifecycle dispatch.
		/// @param pluginName Plugin's registered name.
		/// @return Obtains a plugin instance and registers it for engine lifecycle dispatch.
		CORE_API Common::IPlugin* RegisterPlugin(const char* pluginName);
	}
} // namespace Core
