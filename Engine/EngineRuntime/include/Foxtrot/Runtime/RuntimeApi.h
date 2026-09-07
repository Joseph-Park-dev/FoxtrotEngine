#pragma once
#include "Foxtrot/Core/ModuleApi.h"
namespace Foxtrot {
struct RuntimeControl {
    Status (FT_CALL* attach)(const char*, void*) noexcept;
    Status (FT_CALL* run)(uint32_t maxFrames) noexcept;
    void (FT_CALL* stop)() noexcept;
};
}
