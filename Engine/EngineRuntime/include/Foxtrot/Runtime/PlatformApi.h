#pragma once
#include <Windows.h>
#include "Plugin/CoreExports.h"
extern "C" {
/// @brief Creates a native window and associates its callback and user context.
/// @param title Window title or dialog caption.
/// @param width Width of the window, texture, or geometry.
/// @param height Height of the window, texture, or geometry.
/// @param callback Callback invoked by the host with its associated context.
/// @param user Opaque context passed to the callback.
/// @return Creates a native window and associates its callback and user context.
/// @note Call on the thread that owns the native windows.
CORE_API HWND FtCreateNativeWindow(const char* title, unsigned int width, unsigned int height, WNDPROC callback, void* user);
/// @brief Destroys a window registered with the runtime's native window service.
/// @param window Window used by the operation.
/// @note Call on the thread that owns the native windows.
CORE_API void FtDestroyNativeWindow(HWND window) noexcept;
/// @brief Destroys all native windows retained by the runtime service.
/// @note Call on the thread that owns the native windows.
CORE_API void FtDestroyAllNativeWindows() noexcept;
/// @brief Reads the mouse-wheel movement accumulated for the current input frame.
/// @return The mouse-wheel movement accumulated for the current input frame.
CORE_API float FtMouseWheelDelta() noexcept;
/// @brief Clears transient native input accumulators before sampling a new frame.
CORE_API void FtBeginInputFrame() noexcept;
}

/// @brief Disconnects a native window callback before its owning module is unloaded.
/// @param handle Native handle managed by this operation.
/// @note Call on the thread that owns the native windows.
extern "C" CORE_API void FtDetachWindowCallback(HWND handle) noexcept;
