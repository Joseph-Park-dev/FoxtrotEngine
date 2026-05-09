#pragma once
#include "Entity/D3D11Window.h"

namespace Core
{
	class FTRectArea;
}

namespace D3D11
{
	class D3D11Window;
	class D3D11Renderer;
	class D3D11InputDevice;

	using CREATE_WINDOW		 = D3D11::D3D11Window* (*)(const char*, unsigned int, unsigned int, Core::FTRectArea*);
	using CREATE_WINDOW_PROC = D3D11::D3D11Window* (*)(const char*, unsigned int, unsigned int, Core::FTRectArea*, WNDPROC, WNDPROC_Params*);
	using CREATE_RENDERER	 = D3D11::D3D11Renderer* (*)(D3D11::D3D11Window*);
	using CREATE_INPUTDEVICE = D3D11::D3D11InputDevice* (*)();

	namespace PluginKey
	{
		constexpr const char* D3D11				  = "D3D11";
		constexpr const char* CREATE_D3D11_WINDOW = "CreateD3D11Window";
		constexpr const char* CREATE_RENDERER	  = "CreateRenderer";
		constexpr const char* CREATE_INPUTDEVICE  = "CreateInputDevice";
		constexpr const char* CREATE_FTRECTAREA	  = "CreateFTRectArea";
		constexpr const char* SAVE_PROPERTIES	  = "SaveProperties";
		constexpr const char* LOAD_PROPERTIES	  = "LoadProperties";
	} // namespace PluginKey
} // namespace D3D11