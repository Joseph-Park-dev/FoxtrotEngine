// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Engine context that holds references to all core subsystems.
/// Created via IEngineFactory for platform independence.
/// </summary>

#pragma once

namespace Core
{
	class IEngineFactory;
	class IWindow;
	class IRenderer;
	class IInputDevice;
	class ICamera;
	class IResourceManager;

	/// <summary>
	/// Container for engine subsystem instances.
	/// Provides centralized access to all platform-specific objects.
	/// </summary>
	struct EngineContext
	{
		IEngineFactory*    Factory         = nullptr;
		IWindow*           Window          = nullptr;
		IRenderer*         Renderer        = nullptr;
		IInputDevice*      InputDevice     = nullptr;
		ICamera*           Camera          = nullptr;
		IResourceManager*  ResourceManager = nullptr;

		/// <summary>
		/// Releases all subsystems in reverse order of creation.
		/// </summary>
		void Shutdown();
	};
} // namespace Core
