#pragma once

#include <cstddef>

namespace Common
{
	class IActor;
	class IComponent;
	class IPlugin;
}

namespace Core
{
	struct ComponentFactoryEntry
	{
		const char* Name;
		// Creates an actor-owned component and registers it with the supplied plugin.
		Common::IComponent* (*Create)(Common::IPlugin* plugin, Common::IActor* actor);
	};

	/// @brief Optional plugin interface exposing the component types it can create.
	class IComponentFactory
	{
	public:
		static constexpr const char* INTERFACE_NAME = "ComponentFactory";

		/// @brief Returns a borrowed catalogue valid until the plugin is unloaded.
		/// @param count Receives the number of entries; an empty catalogue may return nullptr.
		virtual const ComponentFactoryEntry* GetComponentFactories(std::size_t& count) const noexcept = 0;
		virtual ~IComponentFactory() = default;
	};
}
