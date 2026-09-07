#pragma once
#include "ModuleApi.h"
#ifdef FT_CORE_EXPORTS
#define FT_CORE_API __declspec(dllexport)
#else
#define FT_CORE_API __declspec(dllimport)
#endif
extern "C" {
FT_CORE_API void FT_CALL FtLog(const char* text) noexcept;
FT_CORE_API double FT_CALL FtSeconds() noexcept;
FT_CORE_API void* FT_CALL FtAllocate(uint64_t bytes) noexcept;
FT_CORE_API void FT_CALL FtDeallocate(void* memory) noexcept;
}
