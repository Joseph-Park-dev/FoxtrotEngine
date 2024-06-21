#pragma once
#include <vector>

#include "FoxtrotEngine/Core/TypeDefinitions.h"
#include "FoxtrotEngine/Core/SingletonMacro.h"
#include "FoxtrotEngine/Core/EventType.h"

struct Event
{
	EVENT_TYPE incident;
	ULONG_PTR  lParam;
	ULONG_PTR  wParam;
};

class EventManager
{
	SINGLETON(EventManager);

public:
	void AddEvent(const Event& addedEvent) 
			{ mEvent.push_back(addedEvent); }

private:
	std::vector<Event> mEvent;

public:
	void ProcessEvent();

private:
	void Execute(const Event& executedEvent);
};

