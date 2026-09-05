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
		virtual ~GraphicsFactory() = default;

		//////////////////////////////////////////
		////// Factory Methods ///////////////////
		//////////////////////////////////////////

		virtual IWindow* CreateWindow(
			const char*	 title,
			unsigned int width,
			unsigned int height,
			FTRectArea*	 renderArea) = 0;

		virtual IRenderer* CreateRenderer(IWindow* window) = 0;

		virtual IInputDevice* CreateInputDevice() = 0;

		virtual ICamera* CreateCamera() = 0;

		virtual IGraphicsResourceManager* CreateResourceManager() = 0;

		//////////////////////////////////////////
		////// Backend Information ///////////////
		//////////////////////////////////////////
		virtual const char* GetBackendName() const	   = 0;
		virtual const char* GetBackendVersion() const  = 0;
		virtual bool		IsBackendAvailable() const = 0;
	};
} // namespace Graphics