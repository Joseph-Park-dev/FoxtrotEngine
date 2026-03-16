// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2026 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Interface for Plugins.
/// </summary>

#pragma once
#include "ResourceSystem/FTResource.h"

#include "Static/HashMap.h"

class Actor;
class Component;

using COMP_CONSTRUCTOR = Component* (*)(Actor * actor);

class Plugin : public FTResource
{
public:
	template <typename COMP>
	void AssignComponent(Actor* actor)
	{
		if (!mModule)
		{
			Debug::LogError(__LINE__, __FILE__, "Module is null");
			return;
		}

		FTDS::String procName("Create_");
		procName.Append(COMP::GetName());
		COMP_CONSTRUCTOR compConstruct = GetConstructor(procName.C_Str());
		
		assert(compConstruct);
		mComponents.PushBack(compConstruct(actor));
	}

public:
	void Load();

public:
	virtual bool Initialize();
	virtual void Setup();

	// Gameloop functions.
	virtual void ProcessInput(FTInputDevice* inputDevice);
	virtual void Update(float deltaTime);
	virtual void LateUpdate(float deltaTime);
	virtual void Render(FoxtrotRenderer* renderer);

	virtual void Clear();

public:
	Plugin(FTResourceDef& resDef, size_t compArrCount, size_t compCount);
	~Plugin() override;

protected:
	COMP_CONSTRUCTOR GetConstructor(const char* compName);

private:
	HMODULE							mModule;
	FTDS::DynamicArray<Component*>* mComponents;
	// FTDS::HashMap<Component* (*)(Actor * actor)>* mCompMap;
};
