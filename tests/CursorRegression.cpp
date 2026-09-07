#include "Core/InterfaceAliases.h"
#include "InputSystem/D3D11InputDevice.h"
#include "Factory/IGraphicsFactory.h"
#include "Plugin/IPlugin.h"
#include "Renderer/D3D11Window.h"
#include "Renderer/D3D11Renderer.h"
#include "Renderer/ViewportRenderer.h"
#include "Renderer/IRenderer.h"
#include "Renderer/FTRectArea.h"
#include "Foxtrot/Runtime/PlatformApi.h"
#include <cstdio>
#include <memory>
#include <stdexcept>

static void Require(bool result, const char* message) {
    if (!result) throw std::runtime_error(message);
}

int main() {
    try {
        D3D11::D3D11InputDevice input;
        MSG move{};
        move.message = WM_MOUSEMOVE;
        move.lParam = MAKELPARAM(-27, -13);
        input.DetectMouseInput(move);
        Require(input.MOUSE_X() == -27 && input.MOUSE_Y() == -13, "Negative cursor coordinates wrapped");
        move.lParam = 0;
        input.DetectMouseInput(move);
        Require(input.MOUSE_X() == 0 && input.MOUSE_Y() == 0, "Client origin was ignored");
        MSG wheel{};
        wheel.message = WM_MOUSEWHEEL;
        wheel.lParam = MAKELPARAM(1200, 900); // wheel messages carry screen coordinates
        input.DetectMouseInput(wheel);
        Require(input.MOUSE_X() == 0 && input.MOUSE_Y() == 0, "Screen coordinates replaced client coordinates");

        using Create = Common::IPlugin* (*)(const char*);
        auto create = reinterpret_cast<Create>(GetProcAddress(GetModuleHandleW(L"FoxtrotD3D11.dll"), "CreatePlugin"));
        Require(create != nullptr, "Graphics factory missing");
        std::unique_ptr<Common::IPlugin> plugin(create("CursorRegression"));
        auto factory = static_cast<Core::IGraphicsFactory*>(plugin->QueryInterface("GraphicsFactory"));
        Require(factory != nullptr, "Graphics interface missing");
        // Deliberately use a scene resolution unlike the native client resolution.
        auto window = static_cast<D3D11::D3D11Window*>(factory->CreateAppWindow(
            "Cursor regression", 640, 480, new D3D11::FTRectArea(0, 0, 320, 240)));
        ShowWindow(window->GetHandle(), SW_HIDE);
        auto renderer = factory->CreateRenderer(window);
        {
            D3D11::ViewportRenderer scene;
            auto backend = static_cast<D3D11::D3D11Renderer*>(renderer);
            scene.InitializeTexture(backend, 320, 240);
            scene.BeginRender(backend);
            float x, y, width, height;
            renderer->GetViewport(x, y, width, height);
            Require(width == 320 && height == 240, "Scene viewport inherited native window dimensions");
            scene.EndRender(backend);
        }
        auto verify = [&] {
            window->BeginRender(renderer);
            RECT client{};
            Require(GetClientRect(window->GetHandle(), &client) != FALSE, "Client rectangle unavailable");
            DXGI_SWAP_CHAIN_DESC desc{};
            Require(SUCCEEDED(window->GetSwapChain()->GetDesc(&desc)), "Swapchain description unavailable");
            float x, y, width, height;
            renderer->GetViewport(x, y, width, height);
            Require(desc.BufferDesc.Width == static_cast<UINT>(client.right) &&
                    desc.BufferDesc.Height == static_cast<UINT>(client.bottom), "Back buffer does not match client pixels");
            Require(x == 0 && y == 0 && width == client.right && height == client.bottom,
                    "Presentation viewport does not match client pixels");
            window->EndRender(renderer);
        };
        verify();
        Require(SetWindowPos(window->GetHandle(), nullptr, 0, 0, 913, 677,
            SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE) != FALSE, "Resize failed");
        verify();
        plugin.reset(); // Factory owns window, renderer and render area.
        FtDestroyAllNativeWindows();
        std::puts("PASS cursor coordinates and resized presentation alignment");
        return 0;
    } catch (const std::exception& error) {
        std::fprintf(stderr, "%s\n", error.what());
        return 1;
    }
}
