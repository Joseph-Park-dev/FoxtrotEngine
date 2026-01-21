#pragma once

namespace Path
{
	namespace Resource
	{
		constexpr const char* SHADERS	 = ".\\FoxtrotEngine\\Assets\\Shaders\\";
		constexpr const char* SHADERS_2D = ".\\FoxtrotEngine\\Assets\\Shaders\\2D\\";
	} // namespace Resource

#ifdef FOXTROT_EDITOR
	constexpr const char* EDITOR_CONFIG = ".\\editor.ftconfig";
#endif // FOXTROT_EDITOR
} // namespace Path