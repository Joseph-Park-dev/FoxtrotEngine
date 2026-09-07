#pragma once
#include <cstdint>

// Only fixed-layout values, borrowed byte ranges and opaque pointers cross the
// bootstrap boundary. Never retain an API after its DLL has been released.
#define FT_CALL __cdecl
#define FT_EXPORT extern "C" __declspec(dllexport)
namespace Foxtrot
{
constexpr uint32_t ModuleAbi = 1;
#if defined(FOXTROT_EDITOR)
constexpr uint32_t BuildAbi = 3;
#elif defined(_DEBUG)
constexpr uint32_t BuildAbi = 1;
#else
constexpr uint32_t BuildAbi = 2;
#endif
enum class Status : int32_t { Ok, InvalidArgument, AbiMismatch, Failed, Busy };
struct ModuleApi;
struct HostServices
{
    uint32_t size;
    void* context;
    const ModuleApi* (FT_CALL* find)(void*, const char*) noexcept;
};
struct ModuleApi
{
    uint32_t size;
    uint32_t abi;
    uint32_t build;
    uint32_t pointerBytes;
    const char* name;
    void* instance;
    Status (FT_CALL* initialize)(void*, const HostServices*) noexcept;
    void (FT_CALL* shutdown)(void*) noexcept;
    void (FT_CALL* destroy)(void*) noexcept;
    void* (FT_CALL* query)(void*, const char*) noexcept;
};
using GetModuleApi = Status (FT_CALL*)(uint32_t, uint32_t, ModuleApi*) noexcept;
}
