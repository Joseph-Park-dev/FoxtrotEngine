// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// A singleton manager that is responsible to execute the enqueued events.
/// </summary>

#pragma once
#include "Plugin/CoreExports.h"
#include "Entity/Entity.h"

#include "Utility/SingletonMacro.h"
#include "FTDS/Dynamic/DynamicArray.h"

namespace Core
{
	enum class EVENT_TYPE;

	struct FTEvent
	{
		EVENT_TYPE incident;
		void*	   eventData;
	};

	class CORE_API EventManager
	{
		SINGLETON_PROTECTED(EventManager)

	public:
		/// @brief Queues an event for processing at the frame boundary.
		/// @param addedEvent Event copied into the pending queue.
		void AddEvent(const FTEvent& addedEvent);
		/// @brief Processes queued lifecycle and scene changes at the frame boundary.
		void ProcessEvent();

	protected:
		/// @brief Applies the requested event or command to engine state.
		/// @param executedEvent Event whose operation is applied.
		virtual void Execute(const FTEvent& executedEvent);

	private:
		Common::FTDS::DynamicArray<FTEvent>* mEvent;
	};
} // namespace Core
