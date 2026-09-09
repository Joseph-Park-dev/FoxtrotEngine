#pragma once
#include <cstring>
#include <Debugging/DebugMemAlloc.h>

namespace Common
{
	namespace FTDS
	{
		/// @brief Allocates an independent null-terminated copy of the source text.
		/// @param source Source text or object copied by the operation.
		/// @return New character buffer owned by the caller; release it with delete[].
		inline const char* MakeHeapStr(const char* source)
		{
			size_t size	  = std::strlen(source) + 1;
			char*  buffer = DBG_NEW char[size];
			strcpy_s(buffer, size, source); // Copy the content into the buffer.
			return buffer;
		}
	} // namespace FTDS
} // namespace Common
