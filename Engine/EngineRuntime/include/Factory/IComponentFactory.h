#pragma once

#include <cstddef>

namespace Common
{
	class IActor;
	class IComponent;
	class IPlugin;
}

using ComponentFactoryEntry = Common::IComponent* (*)(Common::IActor * actor);

template <typename Component>
Common::IComponent* CreateRegisteredComponent(Common::IActor* actor)
{
	if (!actor || !actor->GetData() || !actor->GetComponents())
		return nullptr;
	return actor->GetData()->AddComponent<Component>();
}

namespace Core
{
	/// @brief Optional plugin interface exposing the component types it can create.
	class ComponentFactory
	{
	static

	private:
		Common::FTDS::HashMap<ComponentFactoryEntry> mComponentFactories;
	};
}
