#pragma once
#include <vector>

#include "Core/TypeDefinitions.h"
#include "Core/SingletonMacro.h"
#include "Core/EventType.h"

struct FTEvent
{
	EVENT_TYPE incident;
	ULONG_PTR  lParam;
	ULONG_PTR  wParam;
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

