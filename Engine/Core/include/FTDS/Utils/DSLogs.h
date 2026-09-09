#pragma once
#include <cstdio>
#include <cstdlib>
#include <cassert>

namespace Common
{
	/// @brief Writes a formatted error diagnostic to standard output.
	/// @param message Diagnostic text or message identifier.
	inline void LogError(char* message)
	{
		printf_s("%s\n", message);
		exit(1);
	}
} // namespace Common
