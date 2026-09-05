#pragma once

namespace Core
{
	template <typename FUNC>
	inline FUNC GetFunc(const char* moduleName, const char* procName)
	{
		HMODULE coreMod = GetModuleHandleA(moduleName);
		assert(coreMod != NULL);
		FARPROC proc = GetProcAddress(coreMod, procName);
		return reinterpret_cast<FUNC>(proc);
	}
} // namespace Core