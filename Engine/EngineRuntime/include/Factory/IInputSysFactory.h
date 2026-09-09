// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Abstract factory interface for creating platform-specific engine objects.
/// </summary>

#pragma once

namespace Common
{
	namespace FTDS
	{
		class String;
	}
} // namespace Common

namespace InputSystem
{
	class IInputDevice;
} // namespace InputSystem

namespace Core
{
	/// <summary>
	/// Abstract factory for creating platform-specific engine subsystems.
	/// Allows Core to create objects without knowing about specific implementations.
	/// </summary>
	class IInputSysFactory
	{
	public:
		//////////////////////////////////////////
		////// Factory Methods ///////////////////
		//////////////////////////////////////////

		/// @brief Creates the native input device used by the engine.
		/// Creates a platform-specific input device handler.
		/// @return Created input device instance or resource.
		virtual InputSystem::IInputDevice* CreateInputDevice() = 0;

	public:
		/// @brief Completes destruction through the object's inheritance hierarchy.
		virtual ~IInputSysFactory() = default;
	};

} // namespace Core
