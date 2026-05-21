// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Base class for all Components.
///
/// [Design philosophy]
/// The Component controls its elements in an upper level,
/// and the element has the related operation.
///
/// Means of access to the elements (Getters & Setters) are not to be
/// exposed. Make them protected, or minimize the cases of them being public.
/// </summary>

#pragma once
#include <iosfwd>

#include "Debugging/DebugMemAlloc.h"
#include "Plugin/CoreExports.h"

namespace Core
{
	class IActor;
	class IInputDevice;

	namespace DefaultVal
	{
		constexpr int UPDATE_ORDER = 100;
	}

	namespace ChunkKey
	{
		constexpr const char* COMPONENTS   = "Components";
		constexpr const char* UPDATE_ORDER = "UpdateOrder";
		constexpr const char* IS_ACTIVE	   = "Is Active";
	} // namespace ChunkKey

	class IComponent
	{
	public:
		virtual const char* GetName() const = 0;

	public:
		virtual void Initialize()							 = 0;
		virtual void Setup()								 = 0;
		virtual void ProcessInput(IInputDevice* inputDevice) = 0;
		virtual void Update(float deltaTime)				 = 0;
		virtual void LateUpdate(float deltaTime)			 = 0;
		virtual void CloneTo(IActor* actor)					 = 0;

	public:
		virtual void SaveProperties(std::ofstream& ofs) = 0;
		virtual void LoadProperties(std::ifstream& ifs) = 0;

	public:
		virtual IActor*	   GetOwner()				= 0;
		virtual const int  GetUpdateOrder()			= 0;
		virtual const bool GetIsInitialized() const = 0;
		virtual const bool GetIsSetup() const		= 0;
		virtual const bool GetIsActive() const		= 0;

		virtual void SetIsActive(bool isActive) = 0;

		// virtual int& UpdateOrder() = 0;

	public:
		template <class T>
		static void Load(Core::IActor* actor, std::ifstream& ifs)
		{
			/// Dynamically allocate actor of type T
			T* t = DBG_NEW T(actor, DefaultVal::UPDATE_ORDER);
			// Load Properties first -> then initialize with the loaded values.
			t->LoadProperties(ifs);
		}
	};

	template <typename COMP>
	COMP CreateComponent(Core::IActor* owner, int updateOrder)
	{
		return DBG_NEW COMP;
	}
} // namespace Core