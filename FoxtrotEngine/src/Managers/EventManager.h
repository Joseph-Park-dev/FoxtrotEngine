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
#include <vector>

#include "Core/TypeDefinitions.h"
#include "Core/SingletonMacro.h"
#include "Core/EventType.h"

struct FTEvent
{
	EVENT_TYPE incident;
	std::vector<void*> eventData;
};

class EventManager
{
	SINGLETON(EventManager);

public:
	void AddEvent(const FTEvent& addedEvent)
			{ mEvent.push_back(addedEvent); }

private:
	std::vector<FTEvent> mEvent;

public:
	void ProcessEvent();

private:
	void Execute(const FTEvent& executedEvent);
};

