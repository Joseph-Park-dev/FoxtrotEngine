#pragma once

namespace Common
{
	namespace DLLPath
	{
		constexpr const char* CORE	= "Core.dll";
		constexpr const char* D3D11 = "D3D11.dll";

#ifdef FOXTROT_EDITOR
		constexpr const char* CORE_EDITOR  = "Core_Editor.dll";
		constexpr const char* D3D11_EDITOR = "D3D11_Editor.dll";
		constexpr const char* EDITOR	   = "Editor.dll";
#endif // FOXTROT_EDITOR
	} // namespace DLLPath
} // namespace Common