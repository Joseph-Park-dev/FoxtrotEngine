#pragma once
#include <cstdio>
#include <cstdlib>
#include <cassert>

inline void LogError(char* message)
{
	printf_s("%s\n", message);
	exit(1);
}