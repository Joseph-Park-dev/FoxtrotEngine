#include "Entity/D3D11Window.h"

#include <Windows.h>
#include <wrl.h>

#include "Renderer/D3D11Renderer.h"
#include "Utility/D3D11Utils.h"
#include "InputSystem/D3D11InputDevice.h"
#include "Renderer/FTRectArea.h"
#include "TemplateFunctions.h"
#include "Debugging/DebugFuncs.h"
#include "DebugFuncs.h"
#include "Plugin/IPlugin.h"

namespace D3D11
{
	using namespace Core;
	using namespace Microsoft::WRL;

	bool D3D11Window::Initialize(int windowMode)
	{
		return Initialize(windowMode, D3D11Window::WinProc);
	}

	bool D3D11Window::Initialize(int windowMode, WNDPROC proc, WNDPROC_Params* params)
	{
		assert(!mTitle->IsEmpty());

		const wchar_t* title = mTitle->WC_Str();

		WNDCLASSEX wc = {
			sizeof(WNDCLASSEX),
			CS_CLASSDC,
			proc,
			0L,
			0L,
			GetModuleHandle(NULL),
			NULL,
			NULL,
			NULL,
			NULL,
			title, // lpszClassName, L-string
			NULL
		};
		if (!RegisterClassEx(&wc))
		{
			LogString("RegisterClassEx() failed.");
			return false;
		}
		RECT wr = { 0, 0, static_cast<LONG>(GetWidth()), static_cast<LONG>(GetHeight()) };

		// Calculate required outer window rectangle for given client area.
		AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, false);

		mWinHandle = CreateWindow(
			wc.lpszClassName,
			title,
			WS_OVERLAPPEDWINDOW | WS_SYSMENU,
			100,				// x-coordinate, top left
			100,				// y-coordinate, top left
			wr.right - wr.left, // horizontal resolution
			wr.bottom - wr.top, // vertical resolution
			NULL,
			NULL,
			wc.hInstance,
			params);

		if (!mWinHandle)
		{
			LogString("CreateWindow() failed.");
			return false;
		}

		ShowWindow(mWinHandle, windowMode);
		SetForegroundWindow(mWinHandle);
		UpdateWindow(mWinHandle);

		delete[] title;
		return true;
	}

	bool D3D11Window::InitializeWindowRenderer(D3D11::D3D11Renderer* renderer)
	{
		if (!CreateRTV(renderer->GetDevice()))
		{
			Debug::LogError(__LINE__, __FILE__, "Failed to Initialize RTV");
			return false;
		}

		if (!CreateDSV(renderer->GetDevice(), renderer->GetNumQualityLevels()))
		{
			Debug::LogError(__LINE__, __FILE__, "Failed to Initialize DSV");
			return false;
		}
		return true;
	}

	bool D3D11Window::CreateSwapChain(D3D11::D3D11Renderer* renderer)
	{
		HRESULT hr = D3D11Utils::CreateSwapChain(mWinHandle, renderer->GetDevice(), mSwapChain, GetWidth(), GetHeight(), renderer->GetNumQualityLevels());
		if (hr != S_OK)
			return false;
		return true;
	}

	void D3D11Window::ResizeWindow(Core::IRenderer* renderer)
	{
		D3D11Renderer* rend = reinterpret_cast<D3D11Renderer*>(renderer);
		Reset();
		if (mSwapChain)
		{
			DX::ThrowIfFailed(
				mSwapChain->ResizeBuffers(0, GetWidth(), GetHeight(), DXGI_FORMAT_UNKNOWN, 0));

			CreateRTV(rend->GetDevice());
			CreateDSV(rend->GetDevice(), rend->GetNumQualityLevels());
		}
	}

	void D3D11Window::BeginRender(Core::IRenderer* renderer)
	{
		D3D11Renderer* rend = reinterpret_cast<D3D11Renderer*>(renderer);
		ClearWindow(rend);
		ID3D11RenderTargetView* targetsPrev[] = { mRTV.Get() };
		rend->GetContext()->OMSetRenderTargets(1, targetsPrev, mDSV.Get());
		// renderer->GetContext()->OMSetDepthStencilState(renderer->GetDSS().Get(), 0);

		MSG msg = {};
		InvalidateRect(mWinHandle, NULL, true);
		if (PeekMessage(&msg, mWinHandle, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		UpdateWindow(mWinHandle);
		rend->SetViewport(0.f, 0.f, GetRenderArea()->GetSize().x, GetRenderArea()->GetSize().y);
	}

	void D3D11Window::EndRender(Core::IRenderer* renderer)
	{
		mSwapChain->Present(1, 0);

		D3D11Renderer*			rend		= reinterpret_cast<D3D11Renderer*>(renderer);
		ID3D11RenderTargetView* nullViews[] = { nullptr };
		rend->GetContext()->OMSetRenderTargets(1, nullViews, nullptr);
		rend->GetContext()->OMSetDepthStencilState(nullptr, 0);
	}

	void D3D11Window::Reset()
	{
		mRTV.Reset();
		mDSV.Reset();
	}

	HWND&							D3D11Window::GetHandle() { return mWinHandle; }
	ComPtr<IDXGISwapChain>&			D3D11Window::GetSwapChain() { return mSwapChain; }
	ComPtr<ID3D11RenderTargetView>& D3D11Window::GetRTV() { return mRTV; }
	ComPtr<ID3D11DepthStencilView>& D3D11Window::GetDSV() { return mDSV; }

	const Common::FTDS::String* D3D11Window::GetTitle() const
	{
		return mTitle;
	}

	unsigned int D3D11Window::GetWidth() const
	{
		return mWidth;
	}

	unsigned int D3D11Window::GetHeight() const
	{
		return mHeight;
	}

	void D3D11Window::SetWidth(unsigned int width)
	{
		mWidth = width;
	}

	void D3D11Window::SetHeight(unsigned int height)
	{
		mHeight = height;
	}

	FTRectArea* D3D11Window::GetRenderArea() const
	{
		return mRenderArea;
	}

	D3D11Window::D3D11Window(const char* title, unsigned int width, unsigned int height, FTRectArea* rndArea)
		: mTitle(DBG_NEW Common::FTDS::String(title))
		, mWidth(width)
		, mHeight(height)
		, mRenderArea(rndArea)
	{
		this->Initialize(WS_OVERLAPPED);
	}

	D3D11Window::D3D11Window(const char* title, unsigned int width, unsigned int height, FTRectArea* rndArea, WNDPROC proc, WNDPROC_Params* params)
		: mTitle(DBG_NEW Common::FTDS::String(title))
		, mWidth(width)
		, mHeight(height)
		, mRenderArea(rndArea)
	{
		this->Initialize(WS_OVERLAPPED, proc, params);
	}

	D3D11Window::~D3D11Window()
	{
		delete mTitle;
		delete mRenderArea;
	}

	bool D3D11Window::CreateRTV(Microsoft::WRL::ComPtr<ID3D11Device>& device)
	{
		HRESULT hr = D3D11Utils::CreateRenderTargetView(mRTV, device, mSwapChain);
		if (hr != S_OK)
			return false;
		return true;
	}

	bool D3D11Window::CreateDSV(Microsoft::WRL::ComPtr<ID3D11Device>& device, unsigned int numQualityLevels)
	{
		HRESULT hr = D3D11Utils::CreateDepthBuffer(device, GetWidth(), GetHeight(), numQualityLevels, mDSV);
		if (hr != S_OK)
			return false;
		return true;
	}

	void D3D11Window::ClearWindow(D3D11Renderer* renderer)
	{
		FLOAT clearColor[4] = { 0.f, 0.f, 0.f, 0.f };
		if (mRTV)
			renderer->GetContext()->ClearRenderTargetView(mRTV.Get(), clearColor);
		if (mDSV)
			renderer->GetContext()->ClearDepthStencilView(mDSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}

	LRESULT D3D11Window::WinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		Core::FTCore* core = nullptr;
		if (msg == WM_NCCREATE)
		{
			auto* cs = reinterpret_cast<CREATESTRUCT*>(lParam);
			core	 = static_cast<Core::FTCore*>(cs->lpCreateParams);
			SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(core));
		}
		else
		{
			// Retrieve the pointer on every message
			core = reinterpret_cast<Core::FTCore*>(
				GetWindowLongPtr(hwnd, GWLP_USERDATA));
		}
		if (msg == WM_DESTROY)
		{
			return 0;
		}

		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
} // namespace D3D11