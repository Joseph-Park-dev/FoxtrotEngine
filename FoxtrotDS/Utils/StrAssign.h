#pragma once
#include <cstring>

namespace FTDS
{
	inline const char* MakeHeapStr(const char* source)
	{
		size_t size = std::strlen(source) + 1;
		char* buffer = DBG_NEW char[size];
		strcpy_s(buffer, size, source);  // Copy the content into the buffer.
		return buffer;
	}
} // namespace FTDS