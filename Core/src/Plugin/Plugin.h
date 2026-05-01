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

namespace Core
{
	class Actor;
	class FTInputDevice;
	class FoxtrotRenderer;
	class Component;

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
		virtual void SaveProperties()					 = 0;
		virtual void LoadProperties()					 = 0;
		virtual void SaveManagerData(std::ofstream& ofs) = 0;
		virtual void LoadManagerData(std::ifstream& ifs) = 0;

	public:
		const FTDS::String*				GetName() { return mName; }
		HMODULE&						GetModule() { return mModule; }
		FTDS::DynamicArray<Component*>* GetRegisteredComps() { return mRegisteredComps; }

	public:
		virtual void Initialize()
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

		virtual void ProcessInput() = 0;
		virtual void Render()		= 0;
		virtual void ProcessEvent() = 0;
		virtual void ShutDown()		= 0;

		virtual void Update(float deltaTime)
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

		void Clear()
		{
			if (0 < mRegisteredComps->GetSize())
				mRegisteredComps->Clear();
			delete mRegisteredComps;
		}

	public:
		void SetModule(HMODULE module)
		{
			mModule = module;
		}

	public:
		Plugin(const char* name)
			: mName(DBG_NEW FTDS::String(name))
			, mModule(NULL)
			, mRegisteredComps(DBG_NEW FTDS::DynamicArray<Component*>)
		//, mCreateFuncs(DBG_NEW FTDS::DynamicArray<Component* (*)(Actor*)>)
		{
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

	private:
		FTDS::String*					mName;
		HMODULE							mModule;
		FTDS::DynamicArray<Component*>* mRegisteredComps;
		// FTDS::DynamicArray<Component* (*)(Actor*)>* mCreateFuncs;
		//  FTDS::HashMap<Component* (*)(Actor * actor)>* mCompMap;

	private:
		void Unload() const
		{
			FreeLibrary(mModule);
			delete mName;
		}
	};

	namespace ChunkKey
	{
		namespace Plugin
		{
			constexpr const char* PLUGIN_DATA		= "Plugin Data";
			constexpr const char* DLL_PATH			= "DLL Path";
			constexpr const char* COMP_CONSTRUCTORS = "Comp Constructors";
			constexpr const char* MANAGER_DATA		= "Manager Data";
		} // namespace Plugin
	} // namespace ChunkKey

	extern "C" __declspec(dllexport) Plugin* CreatePlugin(const char* name);
} // namespace Core