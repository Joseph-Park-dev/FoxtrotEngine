#pragma once
#include "Utility/SingletonMacro.h"

#include "Manager/EventManager.h"

namespace Editor
{
	class EditorEventManager : public Core::EventManager
	{
		SINGLETON(EditorEventManager)

	private:
		/// @brief Applies the requested event or command to engine state.
		/// @param executedEvent Event whose operation is applied.
		void Execute(const Core::FTEvent& executedEvent) override;
	};
} // namespace Editor
