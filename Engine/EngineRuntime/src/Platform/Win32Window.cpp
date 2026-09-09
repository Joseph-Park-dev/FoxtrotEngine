#include "Foxtrot/Runtime/PlatformApi.h"
#include <memory>
#include <string>
#include <vector>
#include <algorithm>
namespace {
struct Window { HWND handle{}; WNDPROC callback{}; void* user{}; };
std::vector<std::unique_ptr<Window>> windows;
float wheel = 0;
constexpr wchar_t ClassName[] = L"Foxtrot.Runtime.Window";
/// @brief Dispatches native window messages for input and window lifecycle handling.
/// @param hwnd Native window receiving the message.
/// @param msg Windows message containing input or window data.
/// @param wp Message-specific Windows parameter.
/// @param lp Message-specific Windows parameter.
/// @return Dispatches native window messages for input and window lifecycle handling.
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    auto state = reinterpret_cast<Window*>(GetPropW(hwnd, L"Foxtrot.Runtime.State"));
    if (msg == WM_NCCREATE) {
        state = static_cast<Window*>(reinterpret_cast<CREATESTRUCTW*>(lp)->lpCreateParams);
        state->handle = hwnd;
        SetPropW(hwnd, L"Foxtrot.Runtime.State", state);
    }
    if (msg == WM_MOUSEWHEEL) wheel += static_cast<float>(GET_WHEEL_DELTA_WPARAM(wp));
    // Runtime keeps ownership in a separate property; callbacks may use GWLP_USERDATA.
    if (state && state->callback) {
        LRESULT result;
        if (msg == WM_NCCREATE) {
            auto create = *reinterpret_cast<CREATESTRUCTW*>(lp);
            create.lpCreateParams = state->user;
            result = state->callback(hwnd, msg, wp, reinterpret_cast<LPARAM>(&create));
        } else result = state->callback(hwnd, msg, wp, lp);
        return result;
    }
    if (msg == WM_CLOSE) { PostQuitMessage(0); return 0; }
    return DefWindowProcW(hwnd, msg, wp, lp);
}
}
/// @brief Creates a native window and associates its callback and user context.
/// @param title Window title or dialog caption.
/// @param width Width of the window, texture, or geometry.
/// @param height Height of the window, texture, or geometry.
/// @param callback Callback invoked by the host with its associated context.
/// @param user Opaque context passed to the callback.
/// @return Creates a native window and associates its callback and user context.
/// @note Call on the thread that owns the native windows.
extern "C" HWND FtCreateNativeWindow(const char* title, unsigned int width, unsigned int height, WNDPROC callback, void* user) {
    WNDCLASSEXW wc{sizeof(wc)};
    wc.lpfnWndProc = WindowProc; wc.hInstance = GetModuleHandleW(L"EngineRuntime.dll");
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW); wc.lpszClassName = ClassName;
    if (!RegisterClassExW(&wc) && GetLastError() != ERROR_CLASS_ALREADY_EXISTS) return nullptr;
    int count = MultiByteToWideChar(CP_UTF8, 0, title, -1, nullptr, 0);
    if (!count) return nullptr;
    std::wstring wide(static_cast<size_t>(count), L'\0');
    MultiByteToWideChar(CP_UTF8, 0, title, -1, wide.data(), count);
    auto state = std::make_unique<Window>(); state->callback = callback; state->user = user;
    RECT rect{0,0,static_cast<LONG>(width),static_cast<LONG>(height)};
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);
    HWND handle = CreateWindowExW(0,ClassName,wide.c_str(),WS_OVERLAPPEDWINDOW,CW_USEDEFAULT,CW_USEDEFAULT,
        rect.right-rect.left,rect.bottom-rect.top,nullptr,nullptr,wc.hInstance,state.get());
    if (!handle) return nullptr;
    windows.push_back(std::move(state));
    ShowWindow(handle, SW_SHOW);
    return handle;
}
/// @brief Destroys a window registered with the runtime's native window service.
/// @param handle Native handle managed by this operation.
/// @note Call on the thread that owns the native windows.
extern "C" void FtDestroyNativeWindow(HWND handle) noexcept {
    auto it = std::find_if(windows.begin(), windows.end(), [handle](const auto& w){return w->handle==handle;});
    if (it == windows.end()) return;
    (*it)->callback = nullptr;
    RemovePropW(handle, L"Foxtrot.Runtime.State");
    DestroyWindow(handle);
    windows.erase(it);
}
/// @brief Destroys all native windows retained by the runtime service.
/// @note Call on the thread that owns the native windows.
extern "C" void FtDestroyAllNativeWindows() noexcept {
    while (!windows.empty()) FtDestroyNativeWindow(windows.back()->handle);
    UnregisterClassW(ClassName, GetModuleHandleW(L"EngineRuntime.dll"));
}
/// @brief Reads the mouse-wheel movement accumulated for the current input frame.
/// @return The mouse-wheel movement accumulated for the current input frame.
extern "C" float FtMouseWheelDelta() noexcept { return wheel; }
/// @brief Clears transient native input accumulators before sampling a new frame.
extern "C" void FtBeginInputFrame() noexcept { wheel = 0; }

/// @brief Disconnects a native window callback before its owning module is unloaded.
/// @param handle Native handle managed by this operation.
/// @note Call on the thread that owns the native windows.
extern "C" void FtDetachWindowCallback(HWND handle) noexcept {
    for (auto& window : windows) if (window->handle == handle) { window->callback = nullptr; window->user = nullptr; SetWindowLongPtrW(handle, GWLP_USERDATA, 0); }
}
