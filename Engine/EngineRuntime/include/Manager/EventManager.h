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
		void AddEvent(const FTEvent& addedEvent);
		void ProcessEvent();

	protected:
		virtual void Execute(const FTEvent& executedEvent);

	private:
		Common::FTDS::DynamicArray<FTEvent>* mEvent;
	};
} // namespace Core