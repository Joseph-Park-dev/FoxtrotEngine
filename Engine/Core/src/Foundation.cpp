#include "Foxtrot/Core/CoreApi.h"
#include <Windows.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>

extern "C" void FT_CALL FtLog(const char* text) noexcept
{
    if (!text) return;
    std::fprintf(stderr, "%s\n", text);
    OutputDebugStringA(text);
    OutputDebugStringA("\n");
}
extern "C" double FT_CALL FtSeconds() noexcept
{
    LARGE_INTEGER counter{}, frequency{};
    QueryPerformanceCounter(&counter);
    QueryPerformanceFrequency(&frequency);
    return static_cast<double>(counter.QuadPart) / frequency.QuadPart;
}
extern "C" void* FT_CALL FtAllocate(uint64_t bytes) noexcept
{
    if (bytes > SIZE_MAX) return nullptr;
    return std::malloc(static_cast<size_t>(bytes));
}
extern "C" void FT_CALL FtDeallocate(void* memory) noexcept { std::free(memory); }
namespace {
Foxtrot::Status FT_CALL Initialize(void*, const Foxtrot::HostServices*) noexcept
{ FtLog("Core initialized"); return Foxtrot::Status::Ok; }
void FT_CALL Shutdown(void*) noexcept { FtLog("Core shutdown"); }
void FT_CALL Destroy(void*) noexcept {}
void* FT_CALL Query(void*, const char*) noexcept { return nullptr; }
}
FT_EXPORT Foxtrot::Status FT_CALL FtGetModuleApi(uint32_t abi, uint32_t bytes, Foxtrot::ModuleApi* out) noexcept
{
    if (!out || bytes != sizeof(*out) || abi != Foxtrot::ModuleAbi) return Foxtrot::Status::AbiMismatch;
    *out = {sizeof(*out), Foxtrot::ModuleAbi, Foxtrot::BuildAbi, sizeof(void*), "Core", nullptr,
        Initialize, Shutdown, Destroy, Query};
    return Foxtrot::Status::Ok;
}
