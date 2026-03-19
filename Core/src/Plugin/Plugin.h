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

#include <assert.h>
#include <Windows.h>

#include "Debugging/DebugMemAlloc.h"
#include "Debugging/DebugFuncs.h"

#ifdef CORE_EXPORTS
	#define CORE_API __declspec(dllexport)
#else
	#define CORE_API __declspec(dllimport)
#endif

class Actor;
class Component;
class FTInputDevice;
class FoxtrotRenderer;
namespace FTDS
{
	template <typename TYPE>
	class DynamicArray;
	class String;
} // namespace FTDS

using COMP_CONSTRUCTOR = Component* (*)(Actor * actor);

class CORE_API Plugin : public FTResource
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
	virtual void Initialize();
	virtual void Setup();

	// Gameloop functions.
	virtual void ProcessInput(FTInputDevice* inputDevice);
	virtual void Update(float deltaTime);
	virtual void LateUpdate(float deltaTime);
	virtual void Render(FoxtrotRenderer* renderer);

	virtual void Clear();

public:
	Plugin(FTResourceDef& resDef);
	~Plugin() override;

	// public:
	//	void SaveProperties(std::ofstream& ofs) override;
	//	void LoadProperties(std::ifstream& ifs) override;

protected:
	COMP_CONSTRUCTOR GetConstructor(const char* compName);

private:
	HMODULE							mModule;
	FTDS::DynamicArray<Component*>* mComponents;
	// FTDS::HashMap<Component* (*)(Actor * actor)>* mCompMap;

private:
	void Load();
};