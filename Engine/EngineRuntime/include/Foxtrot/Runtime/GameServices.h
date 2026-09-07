#pragma once
#include "Foxtrot/Core/ModuleApi.h"

namespace Foxtrot {
using SystemTick = void (FT_CALL*)(void* context, float deltaSeconds) noexcept;
// Main-thread-only service table. Context and callback remain owned by Game.
// Remove every registration before releasing its owner DLL. Removal while a
// callback is executing returns Busy; retry after the frame has completed.
struct GameServices {
    uint32_t size;
    Status (FT_CALL* registerSystem)(SystemTick, void*, uint64_t* token) noexcept;
    Status (FT_CALL* unregisterSystem)(uint64_t token) noexcept;
    void (FT_CALL* requestExit)() noexcept;
};
}
