#pragma once
#include <iostream>

namespace Common
{
	namespace Debug
	{
		/// @brief Writes a formatted error diagnostic to standard output.
		/// @param line Text line to parse or modify.
		/// @param file Source filename or module file to load.
		/// @param msg Windows message containing input or window data.
		inline void LogError(const int line, const char* file, const char* msg)
		{
			printf("ERROR at line number %d in file %s : %s\n", line, file, msg);
		}

		//inline ULONG GetRefCount(IUnknown* obj)
		//{
		//	obj->AddRef();
		//	ULONG refCount = obj->Release();
		//	return refCount;
		//}
	} // namespace Debug
} // namespace Common
