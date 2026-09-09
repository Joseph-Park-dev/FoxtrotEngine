#pragma once
#include <Windows.h>
#include <cassert>

namespace Core
{
	/// @brief Returns the func used by this service.
	/// @param moduleName Name of the already loaded module.
	/// @param procName Name of the exported function to resolve.
	/// @return Current func.
	template <typename FUNC>
	inline FUNC GetFunc(const char* moduleName, const char* procName)
	{
		HMODULE coreMod = GetModuleHandleA(moduleName);
		assert(coreMod != NULL);
		FARPROC proc = GetProcAddress(coreMod, procName);
		return reinterpret_cast<FUNC>(proc);
	}
} // namespace Core
