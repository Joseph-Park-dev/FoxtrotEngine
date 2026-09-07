#pragma once
#include <Windows.h>
#include "Plugin/CoreExports.h"
extern "C" {
CORE_API HWND FtCreateNativeWindow(const char* title, unsigned int width, unsigned int height, WNDPROC callback, void* user);
CORE_API void FtDestroyNativeWindow(HWND window) noexcept;
CORE_API void FtDestroyAllNativeWindows() noexcept;
CORE_API float FtMouseWheelDelta() noexcept;
CORE_API void FtBeginInputFrame() noexcept;
}

extern "C" CORE_API void FtDetachWindowCallback(HWND handle) noexcept;
