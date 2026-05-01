#pragma once

namespace DLLPaths
{
#ifdef FOXTROT_EDITOR
	constexpr const char* CORE_EDITOR = "Core_Editor.dll";
	constexpr const char* EDITOR	  = "Editor.dll";
#endif // FOXTROT_EDITOR
} // namespace DLLPaths

namespace ProcNames
{
	constexpr const char* CMDHistory = "GetCMDHistory";
} // namespace ProcNames