#pragma once
#include <cstdio>
#include <cstdlib>
#include <cassert>

namespace Common
{
	inline void LogError(char* message)
	{
		printf_s("%s\n", message);
		exit(1);
	}
} // namespace Common