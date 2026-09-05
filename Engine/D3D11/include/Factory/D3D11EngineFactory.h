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
		D3D11EngineFactory() = default;
		~D3D11EngineFactory() override = default;

		//////////////////////////////////////////
		////// Factory Methods ///////////////////
		//////////////////////////////////////////

		Core::IWindow* CreateWindow(
			const char*        title,
			unsigned int       width,
			unsigned int       height,
			D3D11::FTRectArea*  renderArea) override;

		Core::IRenderer* CreateRenderer(Core::IWindow* window) override;

		Core::IInputDevice* CreateInputDevice() override;

		Core::ICamera* CreateCamera() override;

		Core::IResourceManager* CreateResourceManager() override;

		//////////////////////////////////////////
		////// Backend Information ///////////////
		//////////////////////////////////////////

		const char* GetBackendName() const override { return "D3D11"; }
		const char* GetBackendVersion() const override { return "11.0"; }
		bool IsBackendAvailable() const override;
	};
} // namespace D3D11
