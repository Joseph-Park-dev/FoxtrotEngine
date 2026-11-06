#pragma once
#include <vector>
#include "Utility/SingletonMacro.h"

#include "FTDS/Static/HashMap.h"
#include "FTDS/Static/FTString.h"
#include "Plugin/IPlugin.h"
#include "Plugin/PluginKey.h"
#include "Plugin/CoreExports.h"
#include "Foxtrot/Core/ModuleApi.h"

namespace Core
{
	constexpr const char* CREATE_PREFIX = "Create";

	/// @brief Returns the runtime-owned plugin registry without creating a per-DLL singleton.
	CORE_API Common::FTDS::HashMap<Common::IPlugin*>* GetAvailablePlugins();

#pragma warning(push)
#pragma warning(disable : 4251)
	class CORE_API PluginManager
	{
		SINGLETON(PluginManager)

	public:
		// Gameloop functions.
		void Initialize();
		/// @brief Calls Initialize on every component registered to each attached plugin, in attach order.
		void InitializeComponents();
		// Gameloop functions.
		void Setup();
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
		/// @brief Removes a plugin from the registry without deleting it.
		/// @param name Name used when the plugin was attached.
		void Detach(const char* name);

	public:
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

	using PluginReloader = Foxtrot::Status(FT_CALL*)(const char* moduleName) noexcept;

	/// @brief Registers the executable callback that unloads and loads one hosted module.
	/// @param reloader Host callback. Pass nullptr to clear it.
	extern "C" CORE_API void FT_CALL SetPluginReloader(PluginReloader reloader) noexcept;

	/// @brief Asks the executable to unload and load a hosted plugin module.
	/// @param moduleName Module name passed to the host when the DLL was loaded.
	/// @return Ok when the module was reloaded, InvalidArgument when the host does not own it, Busy when it must stay loaded, or Failed.
	extern "C" CORE_API Foxtrot::Status FT_CALL ReloadPluginModule(const char* moduleName) noexcept;

	/// @brief Registers the executable callback that unloads one hosted module.
	/// @param unloader Host callback. Pass nullptr to clear it.
	extern "C" CORE_API void FT_CALL SetPluginUnloader(PluginReloader unloader) noexcept;

	/// @brief Asks the executable to unload a hosted plugin module.
	/// @param moduleName Module name passed to the host when the DLL was loaded.
	/// @return Ok when the module was unloaded, InvalidArgument when the host does not own it, Busy when it must stay loaded, or Failed.
	extern "C" CORE_API Foxtrot::Status FT_CALL UnloadPluginModule(const char* moduleName) noexcept;

	/// @brief Stops later draw calls for a component whose module is about to be unmapped.
	/// @param component Component pointer. Draw checks compare the pointer and do not call it.
	CORE_API void SuspendComponentRendering(const void* component);

	/// @brief Reports whether draw calls for this component were stopped for a DLL reload.
	/// @param component Component pointer recorded by SuspendComponentRendering.
	/// @return True when rendering this component must be skipped.
	CORE_API bool IsComponentRenderingSuspended(const void* component);

	/// @brief Saves a component while its module is still loaded, then removes it from the actor.
	/// @param owner Actor that currently owns the component.
	/// @param component Component whose code is about to be unmapped.
	CORE_API void DetachComponentForModuleReload(Common::IActor* owner, Common::IComponent* component);

	/// @brief Writes component packs captured before a module reload into the open actor save.
	/// @param owner Actor whose captured components should be written.
	CORE_API void AppendReloadedComponentSaves(Common::IActor* owner);

	/// @brief Drops component packs captured for a module reload.
	CORE_API void ClearReloadedComponentSaves();
#pragma warning(pop)
} // namespace Core
