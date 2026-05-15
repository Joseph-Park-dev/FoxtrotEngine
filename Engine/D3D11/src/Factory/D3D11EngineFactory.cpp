// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Factory/D3D11EngineFactory.h"

#include "Debugging/DebugMemAlloc.h"
#include "Entity/D3D11Window.h"
#include "Renderer/D3D11Renderer.h"
#include "InputSystem/D3D11InputDevice.h"
#include "Renderer/Camera.h"
// #include "Manager/ResourceManager.h"  // D3D11::ResourceManager

#include <d3d11.h>

namespace D3D11
{
	Core::IWindow* D3D11EngineFactory::CreateWindow(
		const char*        title,
		unsigned int       width,
		unsigned int       height,
		Core::FTRectArea*  renderArea)
	{
		return DBG_NEW D3D11Window(title, width, height, renderArea);
	}

	Core::IRenderer* D3D11EngineFactory::CreateRenderer(Core::IWindow* window)
	{
		// Safe downcast - factory guarantees window type matches renderer type
		D3D11Window* d3dWindow = static_cast<D3D11Window*>(window);
		return DBG_NEW D3D11Renderer(d3dWindow);
	}

	Core::IInputDevice* D3D11EngineFactory::CreateInputDevice()
	{
		return DBG_NEW D3D11InputDevice();
	}

	Core::ICamera* D3D11EngineFactory::CreateCamera()
	{
		// Camera is a singleton in current design; return the instance
		// Alternatively, create a new camera instance if design changes
		return Camera::GetInstance();
	}

	Core::IResourceManager* D3D11EngineFactory::CreateResourceManager()
	{
		// TODO: Return D3D11::ResourceManager when it implements IResourceManager
		// return D3D11::ResourceManager::GetInstance();
		return nullptr;
	}

	bool D3D11EngineFactory::IsBackendAvailable() const
	{
		// Check if D3D11 is available on this system
		ID3D11Device* testDevice = nullptr;
		D3D_FEATURE_LEVEL featureLevel;

		HRESULT hr = D3D11CreateDevice(
			nullptr,                   // Default adapter
			D3D_DRIVER_TYPE_HARDWARE,  // Hardware device
			nullptr,                   // No software rasterizer
			0,                         // No flags
			nullptr,                   // Default feature levels
			0,                         // Feature level count
			D3D11_SDK_VERSION,
			&testDevice,
			&featureLevel,
			nullptr);

		if (SUCCEEDED(hr) && testDevice)
		{
			testDevice->Release();
			return true;
		}
		return false;
	}
} // namespace D3D11

//////////////////////////////////////////
////// DLL Export ////////////////////////
//////////////////////////////////////////

extern "C" __declspec(dllexport) Core::IEngineFactory* CreateEngineFactory()
{
	return DBG_NEW D3D11::D3D11EngineFactory();
}
