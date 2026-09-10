#include "Foxtrot/Core/CoreApi.h"
#include <Windows.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>

/// @brief Writes a diagnostic message through the core module's logging service.
/// @param text Null-terminated text to display, log, or convert.
extern "C" void FT_CALL FtLog(const char* text) noexcept
{
    if (!text) return;
    std::fprintf(stderr, "%s\n", text);
    OutputDebugStringA(text);
    OutputDebugStringA("\n");
}
/// @brief Samples the high-resolution monotonic clock in seconds.
/// @return Monotonic time in seconds.
extern "C" double FT_CALL FtSeconds() noexcept
{
    LARGE_INTEGER counter{}, frequency{};
    QueryPerformanceCounter(&counter);
    QueryPerformanceFrequency(&frequency);
    return static_cast<double>(counter.QuadPart) / frequency.QuadPart;
}
/// @brief Allocates a block through the core module so allocation and release share an allocator.
/// @param bytes Size of the allocation or ABI output buffer in bytes.
/// @return Allocated block, or nullptr when allocation fails; release it with FtDeallocate().
extern "C" void* FT_CALL FtAllocate(uint64_t bytes) noexcept
{
    if (bytes > SIZE_MAX) return nullptr;
    return std::malloc(static_cast<size_t>(bytes));
}
/// @brief Releases memory obtained from the core module's allocation service.
/// @param memory Memory block previously allocated by the core service.
/// @pre memory must be null or a block returned by FtAllocate().
extern "C" void FT_CALL FtDeallocate(void* memory) noexcept { std::free(memory); }
namespace {
/// @brief Initializes the services and state required before this object's runtime lifecycle begins.
/// @note Unnamed parameter (void*): reserved by this interface or unused by this implementation.
/// @note Unnamed parameter (const Foxtrot::HostServices*): reserved by this interface or unused by this implementation.
/// @return Status indicating whether the operation completed successfully.
Foxtrot::Status FT_CALL Initialize(void*, const Foxtrot::HostServices*) noexcept
{ FtLog("Core initialized"); return Foxtrot::Status::Ok; }
/// @brief Releases initialized services and resources during shutdown.
/// @note Unnamed parameter (void*): reserved by this interface or unused by this implementation.
void FT_CALL Shutdown(void*) noexcept { FtLog("Core shutdown"); }
/// @brief Provides an empty lifecycle or extension hook for this implementation.
/// @note Unnamed parameter (void*): reserved by this interface or unused by this implementation.
void FT_CALL Destroy(void*) noexcept {}
/// @brief Returns the named module service supported by this ABI adapter.
/// @note Unnamed parameter (void*): reserved by this interface or unused by this implementation.
/// @note Unnamed parameter (const char*): reserved by this interface or unused by this implementation.
/// @return Borrowed interface pointer, or nullptr when the interface is unsupported.
void* FT_CALL Query(void*, const char*) noexcept { return nullptr; }
}
/// @brief Validates the requested ABI and publishes this module's lifecycle and service descriptor.
/// @param abi abi used by this operation.
/// @param bytes Size of the allocation or ABI output buffer in bytes.
/// @param out Receives the operation's output.
/// @return ABI status indicating success or the reason the descriptor could not be produced.
FT_EXPORT Foxtrot::Status FT_CALL FtGetModuleAPI(uint32_t abi, uint32_t bytes, Foxtrot::ModuleAPI* out) noexcept
{
    if (!out || bytes != sizeof(*out) || abi != Foxtrot::ModuleAbi) return Foxtrot::Status::AbiMismatch;
    *out = {sizeof(*out), Foxtrot::ModuleAbi, Foxtrot::BuildAbi, sizeof(void*), "Core", nullptr,
        Initialize, Shutdown, Destroy, Query};
    return Foxtrot::Status::Ok;
}
