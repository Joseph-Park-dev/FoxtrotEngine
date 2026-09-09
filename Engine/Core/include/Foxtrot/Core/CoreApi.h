#pragma once
#include "ModuleApi.h"
#ifdef FT_CORE_EXPORTS
#define FT_CORE_API __declspec(dllexport)
#else
#define FT_CORE_API __declspec(dllimport)
#endif
extern "C" {
/// @brief Writes a diagnostic message through the core module's logging service.
/// @param text Null-terminated text to display, log, or convert.
FT_CORE_API void FT_CALL FtLog(const char* text) noexcept;
/// @brief Samples the high-resolution monotonic clock in seconds.
/// @return Monotonic time in seconds.
FT_CORE_API double FT_CALL FtSeconds() noexcept;
/// @brief Allocates a block through the core module so allocation and release share an allocator.
/// @param bytes Size of the allocation or ABI output buffer in bytes.
/// @return Allocated block, or nullptr when allocation fails; release it with FtDeallocate().
FT_CORE_API void* FT_CALL FtAllocate(uint64_t bytes) noexcept;
/// @brief Releases memory obtained from the core module's allocation service.
/// @param memory Memory block previously allocated by the core service.
/// @pre memory must be null or a block returned by FtAllocate().
FT_CORE_API void FT_CALL FtDeallocate(void* memory) noexcept;
}
