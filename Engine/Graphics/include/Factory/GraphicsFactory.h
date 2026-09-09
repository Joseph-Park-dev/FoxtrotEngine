// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// D3D11 implementation of the abstract engine factory.
/// Creates DirectX 11-specific engine subsystems.
/// </summary>

#pragma once
namespace D3D11
{
	class FTRectArea;
} // namespace D3D11

namespace Graphics
{
	class IWindow;
	using FTRectArea = D3D11::FTRectArea;
	class IRenderer;
	class IInputDevice;
	class ICamera;
	class IGraphicsResourceManager;

	class GraphicsFactory
	{
	public:
		/// @brief Completes destruction through the object's inheritance hierarchy.
		virtual ~GraphicsFactory() = default;

		//////////////////////////////////////////
		////// Factory Methods ///////////////////
		//////////////////////////////////////////

		/// @brief Creates the native window and initializes its presentation resources.
		/// @param title Window title or dialog caption.
		/// @param width Width of the window, texture, or geometry.
		/// @param height Height of the window, texture, or geometry.
		/// @param renderArea Area of the window used for rendering.
		/// @return Created window instance or resource.
		virtual IWindow* CreateWindow(
			const char*	 title,
			unsigned int width,
			unsigned int height,
			FTRectArea*	 renderArea) = 0;

		/// @brief Creates the graphics renderer associated with the application window.
		/// @param window Window used by the operation.
		/// @return Created renderer instance or resource.
		virtual IRenderer* CreateRenderer(IWindow* window) = 0;

		/// @brief Creates the native input device used by the engine.
		/// @return Created input device instance or resource.
		virtual IInputDevice* CreateInputDevice() = 0;

		/// @brief Creates a camera instance for scene rendering.
		/// @return Created camera instance or resource.
		virtual ICamera* CreateCamera() = 0;

		/// @brief Creates the backend resource manager for graphics assets.
		/// @return Created resource manager instance or resource.
		virtual IGraphicsResourceManager* CreateResourceManager() = 0;

		/// @brief Returns the backend name used by this graphics factory.
		/// ///////////////////////////////////////
		/// /// Backend Information ///////////////
		/// ///////////////////////////////////////
		/// @return Borrowed access to the backend name.
		virtual const char* GetBackendName() const	   = 0;
		/// @brief Returns the backend version used by this graphics factory.
		/// @return Borrowed access to the backend version.
		virtual const char* GetBackendVersion() const  = 0;
		/// @brief Tests the backend available condition for the current object.
		/// @return True if the operation succeeds or the tested condition holds; otherwise false.
		virtual bool		IsBackendAvailable() const = 0;
	};
} // namespace Graphics
