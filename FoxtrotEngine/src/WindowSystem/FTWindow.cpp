#include "FTWindow.h"

#include <Windows.h>
#include <d3d11.h>

#include "Core/TemplateFunctions.h"
#include "Core/WindowProcess.h"
#include "InputSystem/FTInputDevice.h"
#include "Managers/SceneManager.h"
#include "Renderer/D3D11Utils.h"
#include "Renderer/FoxtrotRenderer.h"
#include "Math/FTMath.h"
#include "Renderer/FTRectArea.h"

#ifdef FOXTROT_EDITOR
	#include <imgui.h>
	#include <imgui_impl_win32.h>
	#include <imgui_impl_dx11.h>

	#include "EditorLayer.h"
	#include "EditorSceneManager.h"
	#include "DirectoryHelper.h"
	#include "ViewportRenderer.h"
#endif

bool FTWindow::InitializeWindow(WNDPROC wndProc)
{
	assert(0 < mWidth || 0 < mHeight);
	assert(!mTitle.empty());

	WNDCLASSEX wc = {
		sizeof(WNDCLASSEX),
		CS_CLASSDC,
		wndProc,
		0L,
		0L,
		GetModuleHandle(NULL),
		NULL,
		NULL,
		NULL,
		NULL,
		mTitle.C_Str(), // lpszClassName, L-string
		NULL
	};
	if (!RegisterClassEx(&wc))
	{
		LogString("RegisterClassEx() failed.");
		return false;
	}
	RECT wr = { 0, 0, mWidth, mHeight };

	// 필요한 윈도우 크기(해상도) 계산
	// wr의 값이 바뀜
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, false);

	mWinHandle = CreateWindow(
		wc.lpszClassName,
		mTitle.C_Str(),
		WS_OVERLAPPEDWINDOW | WS_SYSMENU,
		100,				// x-coordinate, top left
		100,				// y-coordinate, top left
		wr.right - wr.left, // horizontal resolution
		wr.bottom - wr.top, // vertical resolution
		NULL,
		NULL,
		wc.hInstance,
		NULL);

	if (!mWinHandle)
	{
		LogString("CreateWindow() failed.");
		return false;
	}

	ShowWindow(mWinHandle, SW_SHOWDEFAULT);
	SetForegroundWindow(mWinHandle);
	UpdateWindow(mWinHandle);
	return true;
}

bool FTWindow::InitializeWindowRenderer(FoxtrotRenderer* renderer)
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

	if (!CreateIndexRTV(renderer->GetDevice()))
	{
		Debug::LogError(__LINE__, __FILE__, "Failed to Initialize Index RTV");
		return false;
	}
	return true;
}

void FTWindow::SamplCursorPosColor(ComPtr<ID3D11DeviceContext>& context, uint8_t cursorPosColor[4])
{
	// Copies the back buffer data to temp texture.
	if (mIndexTexture && mIndexTempTexture)
	{
		context->ResolveSubresource(mIndexTempTexture.Get(), 0, mIndexTexture.Get(), 0, DXGI_FORMAT_R8G8B8A8_UNORM);

		FTVector2 mousePos = mInputDevice->GetMousePosition();
		if (IsInRenderedArea(mousePos))
		{
			D3D11_BOX box;
			box.left   = mousePos.x;
			box.right  = mousePos.x + 1;
			box.top	   = mousePos.y;
			box.bottom = mousePos.y + 1;
			box.front  = 0;
			box.back   = 1;
			context->CopySubresourceRegion(mIndexStagingTexture.Get(), 0, 0, 0, 0, mIndexTempTexture.Get(), 0, &box);

			D3D11_MAPPED_SUBRESOURCE ms;
			context->Map(mIndexStagingTexture.Get(), NULL, D3D11_MAP_READ, NULL,
						 &ms); // D3D11_MAP_READ 주의

			if (ms.pData)
				memcpy(cursorPosColor, ms.pData, sizeof(uint8_t) * 4);

			context->Unmap(mIndexStagingTexture.Get(), NULL);
		}
	}
}

bool FTWindow::CreateRTV(ComPtr<ID3D11Device>& device)
{
	HRESULT hr = D3D11Utils::CreateRenderTargetView(mRTV, device, mSwapChain);
	if (hr != S_OK)
		return false;

	hr = D3D11Utils::CreateRenderTargetView(mIndexRTV, device, mSwapChain, mIndexTexture, mIndexTempTexture, mIndexStagingTexture);
	if (hr != S_OK)
		return false;

	return true;
}

bool FTWindow::CreateSwapChain(FoxtrotRenderer* renderer)
{
	HRESULT hr = D3D11Utils::CreateSwapChain(mWinHandle, renderer->GetDevice(), mSwapChain, mWidth, mHeight, renderer->GetNumQualityLevels());
	if (hr != S_OK)
		return false;
	return true;
}

bool FTWindow::CreateDSV(ComPtr<ID3D11Device>& device, UINT numQualityLevels)
{
	HRESULT hr = D3D11Utils::CreateDepthBuffer(device, mWidth, mHeight, numQualityLevels, mDSV);
	if (hr != S_OK)
		return false;
	return true;
}

bool FTWindow::CreateIndexRTV(ComPtr<ID3D11Device>& device)
{
	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(textureDesc));

	textureDesc.Width			 = mWidth;
	textureDesc.Height			 = mHeight;
	textureDesc.MipLevels		 = 1;
	textureDesc.ArraySize		 = 1;
	textureDesc.Format			 = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage			 = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags		 = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags	 = 0;
	textureDesc.MiscFlags		 = 0;

	HRESULT hr = device->CreateTexture2D(&textureDesc, NULL, mIndexTempTexture.GetAddressOf());
	hr		   = device->CreateTexture2D(&textureDesc, NULL, mIndexTexture.GetAddressOf());
	if (hr != S_OK)
		return false;

	// Creating 1x1 sized staging texture
	textureDesc.BindFlags	   = 0;
	textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	textureDesc.Usage		   = D3D11_USAGE_STAGING;
	textureDesc.Width		   = 1;
	textureDesc.Height		   = 1;

	hr = device->CreateTexture2D(
		&textureDesc, nullptr, mIndexStagingTexture.GetAddressOf());
	if (hr != S_OK)
		return false;

	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	renderTargetViewDesc.Format				= textureDesc.Format;
	renderTargetViewDesc.ViewDimension		= D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	hr = device->CreateRenderTargetView(
		mIndexTexture.Get(),
		&renderTargetViewDesc,
		mIndexRTV.GetAddressOf());
	if (hr != S_OK)
		return false;
	return true;
}

HWND&							FTWindow::GetHandle() { return mWinHandle; }
ComPtr<IDXGISwapChain>&			FTWindow::GetSwapChain() { return mSwapChain; }
ComPtr<ID3D11RenderTargetView>& FTWindow::GetRTV() { return mRTV; }
ComPtr<ID3D11DepthStencilView>& FTWindow::GetDSV() { return mDSV; }
ComPtr<ID3D11Texture2D>&		FTWindow::GetIndexTexture() { return mIndexTexture; }
ComPtr<ID3D11Texture2D>&		FTWindow::GetIndexTempTexture() { return mIndexTempTexture; }
ComPtr<ID3D11Texture2D>&		FTWindow::GetIndexStagingTexture() { return mIndexStagingTexture; }
ComPtr<ID3D11RenderTargetView>& FTWindow::GetIndexRTV() { return mIndexRTV; }

UINT&		   FTWindow::GetWidth() { return mWidth; }
UINT&		   FTWindow::GetHeight() { return mHeight; }
FTRectArea*	   FTWindow::GetRenderArea() { return mRenderArea; }
FTInputDevice* FTWindow::GetInputDevice() { return mInputDevice; }

void FTWindow::SetWidth(UINT width) { mWidth = width; }
void FTWindow::SetHeight(UINT height) { mHeight = height; }

bool FTWindow::KEY_HOLD(KEY key) { return mInputDevice->GetKeyState(key) == KEY_STATE::HOLD; }
bool FTWindow::KEY_TAP(KEY key) { return mInputDevice->GetKeyState(key) == KEY_STATE::TAP; }
bool FTWindow::KEY_AWAY(KEY key) { return mInputDevice->GetKeyState(key) == KEY_STATE::AWAY; }
bool FTWindow::KEY_NONE(KEY key) { return mInputDevice->GetKeyState(key) == KEY_STATE::NONE; }

bool FTWindow::MOUSE_HOLD(MOUSE mouse) { return mInputDevice->GetMouseState(mouse) == KEY_STATE::HOLD; }
bool FTWindow::MOUSE_TAP(MOUSE mouse) { return mInputDevice->GetMouseState(mouse) == KEY_STATE::TAP; }
bool FTWindow::MOUSE_AWAY(MOUSE mouse) { return mInputDevice->GetMouseState(mouse) == KEY_STATE::AWAY; }
bool FTWindow::MOUSE_NONE(MOUSE mouse) { return mInputDevice->GetMouseState(mouse) == KEY_STATE::NONE; }

FTVector2 FTWindow::MOUSE_POS() { return mInputDevice->GetMousePosition(); }

void FTWindow::ProcessInput()
{
	MSG msg = {};
	if (PeekMessage(&msg, mWinHandle, 0, 0, PM_REMOVE))
	{
		// EditorCamera2D::GetInstance()->ProcessInput(msg);
	}
	mInputDevice->DetectMouseInput(msg);
	mInputDevice->DetectKeyInput();
	TranslateMessage(&msg);
	DispatchMessage(&msg);
}

void FTWindow::ResizeWindow(FoxtrotRenderer* renderer)
{
	Reset(renderer);
	if (mSwapChain)
	{
		DX::ThrowIfFailed(
			mSwapChain->ResizeBuffers(0, mWidth, mHeight, DXGI_FORMAT_UNKNOWN, 0));

		CreateRTV(renderer->GetDevice());
		CreateDSV(renderer->GetDevice(), renderer->GetNumQualityLevels());
		renderer->SetViewport(0, 0, mWidth, mHeight);
	}
}

void FTWindow::Reset(FoxtrotRenderer* renderer)
{
	mRTV.Reset();
	mDSV.Reset();
	mIndexRTV.Reset();

	mIndexTexture.Reset();
	mIndexTempTexture.Reset();
	mIndexStagingTexture.Reset();
}

void FTWindow::BeginRender(FoxtrotRenderer* renderer)
{
	ClearWindow(renderer);
	ID3D11RenderTargetView* targetsPrev[] = { mRTV.Get(),
											  mIndexRTV.Get() };
	renderer->GetContext()->OMSetRenderTargets(2, targetsPrev, mDSV.Get());
	// renderer->GetContext()->OMSetDepthStencilState(renderer->GetDSS().Get(), 0);

	MSG msg = {};
	InvalidateRect(mWinHandle, NULL, true);
	if (PeekMessage(&msg, mWinHandle, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	UpdateWindow(mWinHandle);
}

void FTWindow::EndRender(FoxtrotRenderer* renderer)
{
	ID3D11RenderTargetView* nullViews[] = { nullptr };
	renderer->GetContext()->OMSetRenderTargets(1, nullViews, nullptr);
	renderer->GetContext()->OMSetDepthStencilState(nullptr, 0);
}

void FTWindow::ClearWindow(FoxtrotRenderer* renderer)
{
	if (mRTV)
		renderer->GetContext()->ClearRenderTargetView(mRTV.Get(), mClearColor);
	if (mIndexRTV)
		renderer->GetContext()->ClearRenderTargetView(mIndexRTV.Get(), mClearColor);
	if (mDSV)
		renderer->GetContext()->ClearDepthStencilView(mDSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

bool FTWindow::IsInRenderedArea(FTVector2 pos)
{
	return mRenderArea->Overlaps(pos);
}

FTWindow::FTWindow(const wchar_t* title, UINT width, UINT height)
	: mWinHandle(nullptr)
	, mRenderArea(DBG_NEW FTRectArea(0.f, 0.f, width, height))
	, mInputDevice(DBG_NEW FTInputDevice)
{
	mTitle	= title;
	mWidth	= width;
	mHeight = height;

	mClearColor[0] = 0.3f;
	mClearColor[1] = 0.3f;
	mClearColor[2] = 0.3f;
	mClearColor[3] = 1.0f;
}

FTWindow::~FTWindow()
{
	delete mInputDevice;
	delete mRenderArea;
	DestroyWindow(mWinHandle);
}