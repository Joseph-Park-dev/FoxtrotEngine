#pragma once

namespace Common
{
	class IActor;
	class IComponent;
} // namespace Common

using ComponentFactoryEntry = Common::IComponent* (*)(Common::IActor* actor);

template <typename Component>
Common::IComponent* CreateRegisteredComponent(Common::IActor* actor)
{
	if (!actor || !actor->GetData() || !actor->GetComponents())
		return nullptr;
	return actor->GetData()->AddComponent<Component>();
}
