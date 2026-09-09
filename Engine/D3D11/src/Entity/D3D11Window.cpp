#include "Renderer/D3D11Window.h"

#include <Windows.h>
#include <wrl.h>

#include "Renderer/D3D11Renderer.h"
#include "Utility/D3D11Utils.h"
#include "InputSystem/D3D11InputDevice.h"
#include "Renderer/FTRectArea.h"
#include "TemplateFunctions.h"
#include "Debugging/DebugFuncs.h"
#include "Debugging/D3D11DebugFuncs.h"
#include "Plugin/IPlugin.h"
#include "Foxtrot/Runtime/PlatformApi.h"

namespace D3D11
{
	using namespace Core;
	using namespace Microsoft::WRL;

	/// @brief Initializes the services and state required before this object's runtime lifecycle begins.
	/// @param windowMode Window presentation mode.
	/// @return True if the operation succeeds or the tested condition holds; otherwise false.
	bool D3D11Window::Initialize(int windowMode)
	{
		return Initialize(windowMode, D3D11Window::WinProc);
	}

	/// @brief Initializes the services and state required before this object's runtime lifecycle begins.
	/// @param windowMode Window presentation mode.
	/// @param proc Exported function or window procedure.
	/// @param params Opaque parameters forwarded to the callback.
	/// @return True if the operation succeeds or the tested condition holds; otherwise false.
	bool D3D11Window::Initialize(int windowMode, WNDPROC proc, WNDPROC_Params* params)
	{
        if (mWinHandle) return true;
        mWinHandle = FtCreateNativeWindow(mTitle->C_Str(), mWidth, mHeight,
            proc == D3D11Window::WinProc ? nullptr : proc, params);
        return mWinHandle != nullptr;
    }

	/// @brief Initializes the rendering resources associated with a native window.
	/// @param renderer Renderer providing the graphics device and current render state.
	/// @return True if the operation succeeds or the tested condition holds; otherwise false.
	bool D3D11Window::InitializeWindowRenderer(Graphics::IRenderer* renderer)
	{
		D3D11Renderer* d3dRenderer = static_cast<D3D11Renderer*>(renderer);
		if (!CreateRTV(d3dRenderer->GetDevice()))
		{
			Common::Debug::LogError(__LINE__, __FILE__, "Failed to Initialize RTV");
			return false;
		}

		if (!CreateDSV(d3dRenderer->GetDevice(), d3dRenderer->GetNumQualityLevels()))
		{
			Common::Debug::LogError(__LINE__, __FILE__, "Failed to Initialize DSV");
			return false;
		}
		return true;
	}

	/// @brief Creates the swap chain used to present rendered frames in the native window.
	/// @param renderer Renderer providing the graphics device and current render state.
	/// @return True if the operation succeeds or the tested condition holds; otherwise false.
	bool D3D11Window::CreateSwapChain(Graphics::IRenderer* renderer)
	{
		D3D11Renderer* d3dRenderer = static_cast<D3D11Renderer*>(renderer);
		HRESULT hr = D3D11Utils::CreateSwapChain(mWinHandle, d3dRenderer->GetDevice(), mSwapChain, GetWidth(), GetHeight(), d3dRenderer->GetNumQualityLevels());
		if (hr != S_OK)
			return false;
		return true;
	}

	/// @brief Updates the window presentation resources to match its client dimensions.
	/// @param renderer Renderer providing the graphics device and current render state.
	/// @throws D3D11::DX::com_exception If a checked Direct3D operation fails.
	void D3D11Window::ResizeWindow(Graphics::IRenderer* renderer)
	{
		D3D11Renderer* rend = reinterpret_cast<D3D11Renderer*>(renderer);
        rend->GetContext()->OMSetRenderTargets(0, nullptr, nullptr);
		Reset();
		if (mSwapChain)
		{
			DX::ThrowIfFailed(
				mSwapChain->ResizeBuffers(0, GetWidth(), GetHeight(), DXGI_FORMAT_UNKNOWN, 0));

			CreateRTV(rend->GetDevice());
			CreateDSV(rend->GetDevice(), rend->GetNumQualityLevels());
		}
	}

	/// @brief Binds and clears the render target and configures the viewport for a rendering pass.
	/// @param renderer Renderer providing the graphics device and current render state.
	/// @throws D3D11::DX::com_exception If a checked Direct3D operation fails.
	void D3D11Window::BeginRender(Graphics::IRenderer* renderer)
	{
		D3D11Renderer* rend = reinterpret_cast<D3D11Renderer*>(renderer);
        // Keep presentation pixels aligned with Win32/ImGui client coordinates.
        // Scene render areas are independent offscreen targets.
        RECT client{};
        if (GetClientRect(mWinHandle, &client) && client.right > 0 && client.bottom > 0) {
            DXGI_SWAP_CHAIN_DESC desc{};
            DX::ThrowIfFailed(mSwapChain->GetDesc(&desc));
            SetWidth(static_cast<unsigned int>(client.right));
            SetHeight(static_cast<unsigned int>(client.bottom));
            if (desc.BufferDesc.Width != mWidth || desc.BufferDesc.Height != mHeight)
                ResizeWindow(renderer);
        }
		ClearWindow(rend);
		ID3D11RenderTargetView* targetsPrev[] = { mRTV.Get() };
		rend->GetContext()->OMSetRenderTargets(1, targetsPrev, mDSV.Get());
		// renderer->GetContext()->OMSetDepthStencilState(renderer->GetDSS().Get(), 0);

		rend->SetViewport(0.f, 0.f, static_cast<float>(GetWidth()), static_cast<float>(GetHeight()));
	}

	/// @brief Finishes the rendering pass and restores or presents its target.
	/// @param renderer Renderer providing the graphics device and current render state.
	void D3D11Window::EndRender(Graphics::IRenderer* renderer)
	{
		mSwapChain->Present(1, 0);

		D3D11Renderer*			rend		= reinterpret_cast<D3D11Renderer*>(renderer);
		ID3D11RenderTargetView* nullViews[] = { nullptr };
		rend->GetContext()->OMSetRenderTargets(1, nullViews, nullptr);
		rend->GetContext()->OMSetDepthStencilState(nullptr, 0);
	}

	/// @brief Clears transient state before the next processing cycle.
	void D3D11Window::Reset()
	{
		mRTV.Reset();
		mDSV.Reset();
	}

	/// @brief Returns the handle used by this d3 d11 window.
	/// @return Borrowed access to the handle.
	/// @note Changes through the returned reference affect this object's stored state.
	HWND&							D3D11Window::GetHandle() { return mWinHandle; }
	/// @brief Returns the swap chain used by this d3 d11 window.
	/// @return Borrowed access to the swap chain.
	/// @note Changes through the returned reference affect this object's stored state.
	ComPtr<IDXGISwapChain>&			D3D11Window::GetSwapChain() { return mSwapChain; }
	/// @brief Returns the rtv used by this d3 d11 window.
	/// @return Borrowed access to the rtv.
	/// @note Changes through the returned reference affect this object's stored state.
	ComPtr<ID3D11RenderTargetView>& D3D11Window::GetRTV() { return mRTV; }
	/// @brief Returns the dsv used by this d3 d11 window.
	/// @return Borrowed access to the dsv.
	/// @note Changes through the returned reference affect this object's stored state.
	ComPtr<ID3D11DepthStencilView>& D3D11Window::GetDSV() { return mDSV; }

	/// @brief Returns the title used by this d3 d11 window.
	/// @return Borrowed access to the title.
	const char* D3D11Window::GetTitle() const
	{
		return mTitle ? mTitle->C_Str() : "";
	}

	/// @brief Returns the width used by this d3 d11 window.
	/// @return Current width.
	unsigned int D3D11Window::GetWidth() const
	{
		return mWidth;
	}

	/// @brief Returns the height used by this d3 d11 window.
	/// @return Current height.
	unsigned int D3D11Window::GetHeight() const
	{
		return mHeight;
	}

	/// @brief Updates the width used by subsequent operations.
	/// @param width Replacement width.
	void D3D11Window::SetWidth(unsigned int width)
	{
		mWidth = width;
	}

	/// @brief Updates the height used by subsequent operations.
	/// @param height Replacement height.
	void D3D11Window::SetHeight(unsigned int height)
	{
		mHeight = height;
	}

	/// @brief Returns the render area used by this d3 d11 window.
	/// @return Borrowed access to the render area.
	FTRectArea* D3D11Window::GetRenderArea() const
	{
		return mRenderArea;
	}

	/// @brief Initializes native window and presentation state.
	/// @param title Window title or dialog caption.
	/// @param width Width of the window, texture, or geometry.
	/// @param height Height of the window, texture, or geometry.
	/// @param rndArea Area used for rendering.
	/// @note Initializes the :D3D11Window base or delegates to its constructor.
	D3D11Window::D3D11Window(const char* title, unsigned int width, unsigned int height, FTRectArea* rndArea)
		: mWinHandle(nullptr)
        , mTitle(DBG_NEW Common::FTDS::String(title))
		, mWidth(width)
		, mHeight(height)
		, mRenderArea(rndArea)
	{
		this->Initialize(WS_OVERLAPPED);
	}

	/// @brief Initializes native window and presentation state.
	/// @param title Window title or dialog caption.
	/// @param width Width of the window, texture, or geometry.
	/// @param height Height of the window, texture, or geometry.
	/// @param rndArea Area used for rendering.
	/// @param proc Exported function or window procedure.
	/// @param params Opaque parameters forwarded to the callback.
	/// @note Initializes the :D3D11Window base or delegates to its constructor.
	D3D11Window::D3D11Window(const char* title, unsigned int width, unsigned int height, FTRectArea* rndArea, WNDPROC proc, WNDPROC_Params* params)
		: mWinHandle(nullptr)
        , mTitle(DBG_NEW Common::FTDS::String(title))
		, mWidth(width)
		, mHeight(height)
		, mRenderArea(rndArea)
	{
		this->Initialize(WS_OVERLAPPED, proc, params);
	}

	/// @brief Releases the resources managed by this instance during destruction.
	D3D11Window::~D3D11Window()
	{
		Reset();
        FtDestroyNativeWindow(mWinHandle);
        mWinHandle = nullptr;
        delete mTitle;
		delete mRenderArea;
	}

	/// @brief Creates a render-target view for the supplied texture.
	/// @param device Direct3D device used to create GPU resources.
	/// @return True if the operation succeeds or the tested condition holds; otherwise false.
	bool D3D11Window::CreateRTV(Microsoft::WRL::ComPtr<ID3D11Device>& device)
	{
		HRESULT hr = D3D11Utils::CreateRenderTargetView(mRTV, device, mSwapChain);
		if (hr != S_OK)
			return false;
		return true;
	}

	/// @brief Creates a depth-stencil view for the depth texture.
	/// @param device Direct3D device used to create GPU resources.
	/// @param numQualityLevels Number of supported multisample quality levels.
	/// @return True if the operation succeeds or the tested condition holds; otherwise false.
	bool D3D11Window::CreateDSV(Microsoft::WRL::ComPtr<ID3D11Device>& device, unsigned int numQualityLevels)
	{
		HRESULT hr = D3D11Utils::CreateDepthBuffer(device, GetWidth(), GetHeight(), numQualityLevels, mDSV);
		if (hr != S_OK)
			return false;
		return true;
	}

	/// @brief Clears the window's render target for the next frame.
	/// @param renderer Renderer providing the graphics device and current render state.
	void D3D11Window::ClearWindow(D3D11Renderer* renderer)
	{
		FLOAT clearColor[4] = { 0.f, 0.f, 0.f, 0.f };
		if (mRTV)
			renderer->GetContext()->ClearRenderTargetView(mRTV.Get(), clearColor);
		if (mDSV)
			renderer->GetContext()->ClearDepthStencilView(mDSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}

	/// @brief Dispatches native window messages for input and window lifecycle handling.
	/// @param hwnd Native window receiving the message.
	/// @param msg Windows message containing input or window data.
	/// @param wParam Message-specific Windows parameter.
	/// @param lParam Message-specific Windows parameter.
	/// @return Dispatches native window messages for input and window lifecycle handling.
	LRESULT CALLBACK D3D11Window::WinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
        if (msg == WM_CLOSE) { PostQuitMessage(0); return 0; }

		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
} // namespace D3D11
