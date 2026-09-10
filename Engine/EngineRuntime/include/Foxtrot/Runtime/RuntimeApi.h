#pragma once
#include "Foxtrot/Core/ModuleApi.h"
namespace Foxtrot {
struct RuntimeControl {
    Status (FT_CALL* Attach)(const char*, void*) noexcept;
    Status (FT_CALL* Run)(uint32_t maxFrames) noexcept;
    void (FT_CALL* Stop)() noexcept;
};
}
