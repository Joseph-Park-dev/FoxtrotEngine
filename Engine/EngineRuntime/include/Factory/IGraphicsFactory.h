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

namespace D3D11
{
	class FTRectArea;
} // namespace D3D11

namespace Graphics
{
	class IWindow;
	class IRenderer;
	class ICamera;
} // namespace Graphics

namespace Core
{

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

		/// @brief Creates a native application window with the requested client dimensions and render area.
		/// Creates a platform-specific window.
		/// <param name="title">Window title.</param>
		/// <param name="width">Initial window width.</param>
		/// <param name="height">Initial window height.</param>
		/// <param name="renderArea">Render area within the window (can be nullptr for fullscreen).</param>
		/// @param title Window title or dialog caption.
		/// @param width Width of the window, texture, or geometry.
		/// @param height Height of the window, texture, or geometry.
		/// @param renderArea Area of the window used for rendering.
		/// @return Created app window instance or resource.
		virtual Graphics::IWindow* CreateAppWindow(
			const char*	 title,
			unsigned int width,
			unsigned int height,
			D3D11::FTRectArea*	 renderArea) = 0;

		/// @brief Creates a native application window with the requested client dimensions and render area.
		/// @param title Window title or dialog caption.
		/// @param width Width of the window, texture, or geometry.
		/// @param height Height of the window, texture, or geometry.
		/// @param renderArea Area of the window used for rendering.
		/// @param wndProc Window procedure receiving native messages.
		/// @param wndProcParams Opaque context forwarded to the window procedure.
		/// @return Created app window instance or resource.
		virtual Graphics::IWindow* CreateAppWindow(
			const char*	 title,
			unsigned int width,
			unsigned int height,
			D3D11::FTRectArea*	 renderArea,
			WNDPROC		 wndProc,
			void*		 wndProcParams) = 0;

		/// @brief Creates the graphics renderer associated with the application window.
		/// Creates a platform-specific renderer.
		/// <param name="window">The window to render to.</param>
		/// @param window Window used by the operation.
		/// @return Created renderer instance or resource.
		virtual Graphics::IRenderer* CreateRenderer(Graphics::IWindow* window) = 0;

		/// @brief Creates a camera instance for scene rendering.
		/// Creates a platform-specific camera.
		/// @return Created camera instance or resource.
		virtual Graphics::ICamera* CreateCamera() = 0;

	public:
		/// @brief Completes destruction through the object's inheritance hierarchy.
		virtual ~IGraphicsFactory() = default;
	};

	///// <summary>
	///// Creates the default engine factory for the current platform.
	///// On Windows, this returns a D3D11 factory.
	///// </summary>
	///// <returns>Pointer to the factory. Caller owns the memory.</returns>
	// extern "C" __declspec(dllexport) IGraphicsFactory* CreateGraphicsFactory();
} // namespace Core
