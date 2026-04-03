#pragma once
namespace D3D11
{
	using CREATE_D3D11_WINDOW = void (*)(const char*, unsigned int, unsigned int, FTRectArea*);

	namespace PluginKey
	{
		constexpr const char* D3D11				  = "D3D11";
		constexpr const char* CREATE_D3D11_WINDOW = "CreateD3D11Window";
		constexpr const char* CREATE_FTRECTAREA	  = "CreateFTRectArea";
		constexpr const char* SAVE_PROPERTIES	  = "SaveProperties";
		constexpr const char* LOAD_PROPERTIES	  = "LoadProperties";
	} // namespace PluginKey
} // namespace D3D11