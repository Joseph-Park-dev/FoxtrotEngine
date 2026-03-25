#include "D3D11Window.h"

#include <Windows.h>
#include <wrl.h>

#include "D3D11Renderer.h"
#include "D3D11Utils.h"
#include "D3D11InputDevice.h"
#include "Renderer/FTRectArea.h"
#include "TemplateFunctions.h"
#include "Debugging/DebugFuncs.h"
#include "DebugFuncs.h"
#include "Plugin/Plugin.h"
#include "CoreInstance.h"

bool D3D11Window::Initialize(FTCore* base, int windowMode)
{
	assert(!GetTitle().IsEmpty());

	const wchar_t* title = GetTitle().WC_Str();

	WNDCLASSEX wc = {
		sizeof(WNDCLASSEX),
		CS_CLASSDC,
		D3D11Window::WinProc,
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
		base);

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

bool D3D11Window::Initialize(FTCore* base)
{
	return Initialize(base, SW_SHOWDEFAULT);
}

bool D3D11Window::InitializeWindowRenderer(D3D11Renderer* renderer)
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

bool D3D11Window::CreateSwapChain(D3D11Renderer* renderer)
{
	HRESULT hr = D3D11Utils::CreateSwapChain(mWinHandle, renderer->GetDevice(), mSwapChain, GetWidth(), GetHeight(), renderer->GetNumQualityLevels());
	if (hr != S_OK)
		return false;
	return true;
}

void D3D11Window::ResizeWindow(FoxtrotRenderer* renderer)
{
	D3D11Renderer* rend = static_cast<D3D11Renderer*>(renderer);
	Reset(rend);
	if (mSwapChain)
	{
		DX::ThrowIfFailed(
			mSwapChain->ResizeBuffers(0, GetWidth(), GetHeight(), DXGI_FORMAT_UNKNOWN, 0));

		CreateRTV(rend->GetDevice());
		CreateDSV(rend->GetDevice(), rend->GetNumQualityLevels());
	}
}

void D3D11Window::ProcessInput(D3D11InputDevice* inputDevice)
{
	MSG msg = {};
	if (PeekMessage(&msg, mWinHandle, 0, 0, PM_REMOVE))
	{
		// EditorCamera2D::GetInstance()->ProcessInput(msg);
	}
	inputDevice->DetectMouseInput(msg);
	inputDevice->DetectKeyboardInput();
	TranslateMessage(&msg);
	DispatchMessage(&msg);
}

void D3D11Window::BeginRender(FoxtrotRenderer* renderer)
{
	D3D11Renderer* rend = static_cast<D3D11Renderer*>(renderer);
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

void D3D11Window::EndRender(FoxtrotRenderer* renderer)
{
	mSwapChain->Present(1, 0);

	D3D11Renderer*			rend		= static_cast<D3D11Renderer*>(renderer);
	ID3D11RenderTargetView* nullViews[] = { nullptr };
	rend->GetContext()->OMSetRenderTargets(1, nullViews, nullptr);
	rend->GetContext()->OMSetDepthStencilState(nullptr, 0);
}

void D3D11Window::Reset(FoxtrotRenderer* renderer)
{
	mRTV.Reset();
	mDSV.Reset();
}

HWND&							D3D11Window::GetHandle() { return mWinHandle; }
ComPtr<IDXGISwapChain>&			D3D11Window::GetSwapChain() { return mSwapChain; }
ComPtr<ID3D11RenderTargetView>& D3D11Window::GetRTV() { return mRTV; }
ComPtr<ID3D11DepthStencilView>& D3D11Window::GetDSV() { return mDSV; }

D3D11Window::D3D11Window(Plugin* owner, const char* title, unsigned int width, unsigned int height, FTRectArea* rndArea)
	: FTWindow(owner, title, width, height, rndArea)
{
	this->Initialize(owner->gBase);
}

D3D11Window::~D3D11Window()
{
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
	FTCore* core = nullptr;
	if (msg == WM_NCCREATE)
	{
		auto* cs = reinterpret_cast<CREATESTRUCT*>(lParam);
		core	 = static_cast<FTCore*>(cs->lpCreateParams);
		SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(core));
	}
	else
	{
		// Retrieve the pointer on every message
		core = reinterpret_cast<FTCore*>(
			GetWindowLongPtr(hwnd, GWLP_USERDATA));
	}

	if (core)
	{
		if (msg == WM_DESTROY)
		{
			core->SetIsRunning(false);
			return 0;
		}
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}