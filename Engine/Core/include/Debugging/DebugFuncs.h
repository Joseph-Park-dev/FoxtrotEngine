#pragma once
#include <iostream>

namespace Common
{
	namespace Debug
	{
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