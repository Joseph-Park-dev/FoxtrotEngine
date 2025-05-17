#pragma once
#include <iostream>
#include <Windows.h>

#include "Static/FTString.h"

namespace Debug
{
	inline void LogError(const int line, const char* file, const char* msg)
	{
		printf("ERROR at line number %d in file %s : %s\n", line, file, msg);
	}

	inline void LogError(const int line, const char* file, FTDS::String msg)
	{
		printf("ERROR at line number %d in file %s : %s\n", line, file, msg.C_Str());
	}

	inline ULONG GetRefCount(IUnknown* obj)
	{
		obj->AddRef();
		ULONG refCount = obj->Release();
		return refCount;
	}
} // namespace Debug