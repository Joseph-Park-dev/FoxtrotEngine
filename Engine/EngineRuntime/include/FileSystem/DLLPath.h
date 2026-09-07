#pragma once

namespace Common
{
	namespace DLLPath
	{
		constexpr const char* CORE	= "EngineRuntime.dll";
		constexpr const char* D3D11 = "FoxtrotD3D11.dll";

#ifdef FOXTROT_EDITOR
		constexpr const char* CORE_EDITOR  = "EngineRuntime.dll";
		constexpr const char* D3D11_EDITOR = "FoxtrotD3D11.dll";
		constexpr const char* EDITOR	   = "Editor.dll";
#endif // FOXTROT_EDITOR
	} // namespace DLLPath
} // namespace Common