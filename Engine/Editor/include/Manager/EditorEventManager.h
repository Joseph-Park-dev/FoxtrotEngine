#pragma once
#include "Utility/SingletonMacro.h"

#include "Manager/EventManager.h"

namespace Editor
{
	class EditorEventManager : public Core::EventManager
	{
		SINGLETON(EditorEventManager)

	private:
		void Execute(const Core::FTEvent& executedEvent) override;
	};
} // namespace Editor