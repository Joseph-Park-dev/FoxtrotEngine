// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Abstract factory interface for creating platform-specific engine objects.
/// Each graphics backend (D3D11, Vulkan, etc.) provides its own factory implementation.
/// </summary>

#pragma once
#include <Windows.h>

namespace Common
{
	namespace FTDS
	{
		class String;
	}
} // namespace Common

namespace Core
{
	class IWindow;
	class IRenderer;
	class IInputDevice;
	class ICamera;
	class IResourceManager;
	class FTRectArea;

	/// <summary>
	/// Abstract factory for creating platform-specific engine subsystems.
	/// Allows Core to create objects without knowing about specific implementations.
	/// </summary>
	class IGraphicsFactory
	{
	public:
		//////////////////////////////////////////
		////// Factory Methods ///////////////////
		//////////////////////////////////////////

		/// <summary>
		/// Creates a platform-specific window.
		/// </summary>
		/// <param name="title">Window title.</param>
		/// <param name="width">Initial window width.</param>
		/// <param name="height">Initial window height.</param>
		/// <param name="renderArea">Render area within the window (can be nullptr for fullscreen).</param>
		/// <returns>Pointer to created window. Caller owns the memory.</returns>
		virtual IWindow* CreateAppWindow(
			const char*	 title,
			unsigned int width,
			unsigned int height,
			FTRectArea*	 renderArea) = 0;

		virtual IWindow* CreateAppWindow(
			const char*	 title,
			unsigned int width,
			unsigned int height,
			FTRectArea*	 renderArea,
			WNDPROC		 wndProc,
			void*		 wndProcParams) = 0;

		/// <summary>
		/// Creates a platform-specific renderer.
		/// </summary>
		/// <param name="window">The window to render to.</param>
		/// <returns>Pointer to created renderer. Caller owns the memory.</returns>
		virtual IRenderer* CreateRenderer(IWindow* window) = 0;

		/// <summary>
		/// Creates a platform-specific camera.
		/// </summary>
		/// <returns>Pointer to created camera. Caller owns the memory.</returns>
		virtual ICamera* CreateCamera() = 0;

	public:
		virtual ~IGraphicsFactory() = default;
	};

	///// <summary>
	///// Creates the default engine factory for the current platform.
	///// On Windows, this returns a D3D11 factory.
	///// </summary>
	///// <returns>Pointer to the factory. Caller owns the memory.</returns>
	// extern "C" __declspec(dllexport) IGraphicsFactory* CreateGraphicsFactory();
} // namespace Core
