#pragma once
namespace Core
{
	namespace PluginKey
	{
		constexpr const char* CREATE_PLUGIN = "CreatePlugin";
		namespace FTCore
		{
			constexpr const char* DESTROY = "DestroyCore";
		} // namespace FTCore

	} // namespace PluginKey
} // namespace Core

namespace ProcNames
{
	constexpr const char* CMDHistory = "GetCMDHistory";
	constexpr const char* FIND_ACTOR = "FindActor";
} // namespace ProcNames

namespace Plugin
{
	namespace Name
	{
		constexpr const char* CORE	= "Core";
		constexpr const char* D3D11 = "D3D11";

#ifdef FOXTROT_EDITOR
		constexpr const char* CORE_EDITOR  = "Core_Editor";
		constexpr const char* D3D11_EDITOR = "D3D11_Editor";
		constexpr const char* EDITOR	   = "Editor";
#endif // FOXTROT_EDITOR
	} // namespace Name
} // namespace Plugin