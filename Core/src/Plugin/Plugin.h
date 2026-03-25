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
#include <assert.h>
#include <Windows.h>

#include "Component/Component.h"
#include "Debugging/DebugMemAlloc.h"
#include "Debugging/DebugFuncs.h"
#include "FTDS/Dynamic/DynamicArray.h"
#include "Actor/Actor.h"

#include "Plugin/CoreExports.h"

class Actor;
class FTInputDevice;
class FoxtrotRenderer;

using COMP_CONSTRUCTOR = Component* (*)(Actor * actor);

class Plugin
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

		COMP_CONSTRUCTOR compConstruct = GetConstructor("Create");
		assert(compConstruct);
		mRegisteredComps.PushBack(compConstruct(actor));
	}

public:
	void Initialize()
	{
		for (auto iter = mRegisteredComps->Begin(); iter != mRegisteredComps->End(); ++iter)
			if (!(*iter)->GetIsInitialized())
				(*iter)->Initialize();
	}

	void Setup()
	{
		for (auto iter = mRegisteredComps->Begin(); iter != mRegisteredComps->End(); ++iter)
			if (!(*iter)->GetIsSetup())
				(*iter)->Setup();
	}

	// Gameloop functions.
	void ProcessInput(FTInputDevice* inputDevice)
	{
		for (auto iter = mRegisteredComps->Begin(); iter != mRegisteredComps->End(); ++iter)
		{
			if (!(*iter)->GetOwner()->IsActive())
				continue;
			(*iter)->ProcessInput(inputDevice);
		}
	}
	void Update(float deltaTime)
	{
		for (auto iter = mRegisteredComps->Begin(); iter != mRegisteredComps->End(); ++iter)
		{
			if (!(*iter)->GetOwner()->IsActive())
				continue;
			(*iter)->Update(deltaTime);
		}
	}

	void LateUpdate(float deltaTime)
	{
		for (auto iter = mRegisteredComps->Begin(); iter != mRegisteredComps->End(); ++iter)
		{
			if (!(*iter)->GetOwner()->IsActive())
				continue;
			(*iter)->LateUpdate(deltaTime);
		}
	}
	void Render(FoxtrotRenderer* renderer)
	{
		for (auto iter = mRegisteredComps->Begin(); iter != mRegisteredComps->End(); ++iter)
		{
			if (!(*iter)->GetOwner()->IsActive())
				continue;
			(*iter)->Render(renderer);
		}
	}

	void Clear()
	{
		for (auto iter = mRegisteredComps->Begin(); iter != mRegisteredComps->End(); ++iter)
		{
			delete (*iter);
			*iter = nullptr;
		}
		mRegisteredComps->Clear();
		delete mRegisteredComps;
	}

public:
	Plugin(const wchar_t* dllPath)
		: mRegisteredComps(DBG_NEW FTDS::DynamicArray<Component*>)
		, mCreateFuncs(DBG_NEW FTDS::DynamicArray<Component* (*)(Actor*)>)
	{
		Load(dllPath);
	}

	virtual ~Plugin()
	{
		Clear();
		Unload();
	}

	// public:
	//	void SaveProperties(std::ofstream& ofs) override;
	//	void LoadProperties(std::ifstream& ifs) override;

protected:
	const COMP_CONSTRUCTOR GetConstructor(const char* procName) const
	{
		return (COMP_CONSTRUCTOR)(GetProcAddress(mModule, procName));
	}

private:
	HMODULE										mModule;
	FTDS::DynamicArray<Component*>*				mRegisteredComps;
	FTDS::DynamicArray<Component* (*)(Actor*)>* mCreateFuncs;
	// FTDS::HashMap<Component* (*)(Actor * actor)>* mCompMap;

private:
	void Load(const wchar_t* dllPath)
	{
		mModule = LoadLibrary(dllPath);
	}

	void Unload() const
	{
		FreeLibrary(mModule);
	}
};

extern "C" CORE_API Plugin* CreatePlugin(FTCore* base, const wchar_t* dllPath);