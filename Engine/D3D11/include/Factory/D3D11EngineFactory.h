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
#include "Factory/IEngineFactory.h"

namespace D3D11
{
	class D3D11EngineFactory : public Core::IEngineFactory
	{
	public:
		/// @brief Initializes the backend factory's graphics service context.
		D3D11EngineFactory() = default;
		/// @brief Completes destruction through the object's inheritance hierarchy.
		~D3D11EngineFactory() override = default;

		//////////////////////////////////////////
		////// Factory Methods ///////////////////
		//////////////////////////////////////////

		/// @brief Creates the native window and initializes its presentation resources.
		/// @param title Window title or dialog caption.
		/// @param width Width of the window, texture, or geometry.
		/// @param height Height of the window, texture, or geometry.
		/// @param renderArea Area of the window used for rendering.
		/// @return Created window instance or resource.
		Core::IWindow* CreateWindow(
			const char*        title,
			unsigned int       width,
			unsigned int       height,
			D3D11::FTRectArea*  renderArea) override;

		/// @brief Creates the graphics renderer associated with the application window.
		/// @param window Window used by the operation.
		/// @return Created renderer instance or resource.
		Core::IRenderer* CreateRenderer(Core::IWindow* window) override;

		/// @brief Creates the native input device used by the engine.
		/// @return Created input device instance or resource.
		Core::IInputDevice* CreateInputDevice() override;

		/// @brief Creates a camera instance for scene rendering.
		/// @return Created camera instance or resource.
		Core::ICamera* CreateCamera() override;

		/// @brief Creates the backend resource manager for graphics assets.
		/// @return Created resource manager instance or resource.
		Core::IResourceManager* CreateResourceManager() override;

		//////////////////////////////////////////
		////// Backend Information ///////////////
		//////////////////////////////////////////

		/// @brief Returns the backend name used by this d3 d11 engine factory.
		/// @return Borrowed access to the backend name.
		const char* GetBackendName() const override { return "D3D11"; }
		/// @brief Returns the backend version used by this d3 d11 engine factory.
		/// @return Borrowed access to the backend version.
		const char* GetBackendVersion() const override { return "11.0"; }
		/// @brief Tests the backend available condition for the current object.
		/// @return True if the operation succeeds or the tested condition holds; otherwise false.
		bool IsBackendAvailable() const override;
	};
} // namespace D3D11
