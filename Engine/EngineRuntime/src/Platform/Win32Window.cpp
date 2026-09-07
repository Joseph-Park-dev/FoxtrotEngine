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
extern "C" void FtDestroyNativeWindow(HWND handle) noexcept {
    auto it = std::find_if(windows.begin(), windows.end(), [handle](const auto& w){return w->handle==handle;});
    if (it == windows.end()) return;
    (*it)->callback = nullptr;
    RemovePropW(handle, L"Foxtrot.Runtime.State");
    DestroyWindow(handle);
    windows.erase(it);
}
extern "C" void FtDestroyAllNativeWindows() noexcept {
    while (!windows.empty()) FtDestroyNativeWindow(windows.back()->handle);
    UnregisterClassW(ClassName, GetModuleHandleW(L"EngineRuntime.dll"));
}
extern "C" float FtMouseWheelDelta() noexcept { return wheel; }
extern "C" void FtBeginInputFrame() noexcept { wheel = 0; }

extern "C" void FtDetachWindowCallback(HWND handle) noexcept {
    for (auto& window : windows) if (window->handle == handle) { window->callback = nullptr; window->user = nullptr; SetWindowLongPtrW(handle, GWLP_USERDATA, 0); }
}
