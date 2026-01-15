// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

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
	#define IMGUI_DEFINE_MATH_OPERATORS
	#include <imgui.h>
	#include <imgui_impl_win32.h>
	#include <imgui_impl_dx11.h>

	#include "EditorLayer.h"
	#include "EditorSceneManager.h"
	#include "DirectoryHelper.h"
	#include "ViewportRenderer.h"
#endif

/*
PSEUDOCODE PLAN (Documentation added only):
1. For every public and private method define a Doxygen-style comment summarizing purpose.
2. Include parameter details, return values, side effects, ownership, performance considerations.
3. Explain resource lifecycle (Create*, Reset, destructor) and render loop (BeginRender, EndRender).
4. Add safety notes where assertions or HRESULT checks occur.
5. Keep original logic unchanged.
*/

/**
 * Initializes and registers a Win32 window class, then creates and shows a window.
 * Preconditions:
 *  - mWidth and mHeight must be > 0 (asserted).
 *  - mTitle must be non-empty (asserted).
 * Parameters:
 *  - wndProc: Window procedure callback.
 *  - windowMode: ShowWindow mode flag (e.g., SW_SHOWDEFAULT).
 * Returns:
 *  - true on success, false if class registration or window creation fails.
 * Side Effects:
 *  - Registers a window class (once per unique class name).
 *  - Allocates and destroys a window handle stored in mWinHandle.
 *  - Deletes the wide string buffer returned by mTitle.WC_Str().
 * Notes:
 *  - Title conversion assumes WC_Str() returns a heap-allocated buffer the caller must delete[].
 *  - AdjustWindowRect used to compute outer dimensions for desired client size.
 */
bool FTWindow::InitializeWindow(WNDPROC wndProc, int windowMode)
{
	assert(0 < mWidth || 0 < mHeight);
	assert(!mTitle.IsEmpty());

	const wchar_t* title = mTitle.WC_Str();

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
		title, // lpszClassName, L-string
		NULL
	};
	if (!RegisterClassEx(&wc))
	{
		LogString("RegisterClassEx() failed.");
		return false;
	}
	RECT wr = { 0, 0, static_cast<LONG>(mWidth), static_cast<LONG>(mHeight) };

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
		NULL);

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

/**
 * Convenience overload that uses SW_SHOWDEFAULT for window show mode.
 */
bool FTWindow::InitializeWindow(WNDPROC wndProc)
{
	return InitializeWindow(wndProc, SW_SHOWDEFAULT);
}

/**
 * Initializes Direct3D render targets and depth resources for this window.
 * Parameters:
 *  - renderer: Associated renderer providing ID3D11Device and MSAA quality levels.
 * Returns:
 *  - true if all required resources (RTV, DSV, Index RTV) are created; false otherwise.
 * Failure:
 *  - Logs detailed error via Debug::LogError on each resource creation failure.
 * Notes:
 *  - Must be called after swap chain creation.
 */
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

/**
 * Samples the color (RGBA8) under the current cursor position from an offscreen index texture.
 * Parameters:
 *  - context: Immediate device context used for resolve, copy, and map operations.
 *  - cursorPosColor: Output array of 4 bytes receiving sampled color (if available).
 * Behavior:
 *  - Resolves multisampled (if any) mIndexTexture into mIndexTempTexture.
 *  - Copies a 1x1 box at mouse position into the 1x1 staging texture.
 *  - Maps staging texture for CPU read and copies first 4 bytes into cursorPosColor.
 * Constraints:
 *  - Performs work only if both mIndexTexture and mIndexTempTexture are valid.
 *  - Sampling occurs only if mouse lies within the render area (IsInRenderedArea).
 * Performance:
 *  - Mapping every frame can be costly; consider batching or asynchronous read-back if profiling indicates overhead.
 * Safety:
 *  - Assumes textures created with compatible format DXGI_FORMAT_R8G8B8A8_UNORM.
 */
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
			box.left   = static_cast<UINT>(mousePos.x);
			box.right  = static_cast<UINT>(mousePos.x + 1);
			box.top	   = static_cast<UINT>(mousePos.y);
			box.bottom = static_cast<UINT>(mousePos.y + 1);
			box.front  = 0;
			box.back   = 1;
			context->CopySubresourceRegion(mIndexStagingTexture.Get(), 0, 0, 0, 0, mIndexTempTexture.Get(), 0, &box);

			D3D11_MAPPED_SUBRESOURCE ms;
			context->Map(mIndexStagingTexture.Get(), NULL, D3D11_MAP_READ, NULL,
						 &ms); // D3D11_MAP_READ ÁÖÀÇ

			if (ms.pData)
				memcpy(cursorPosColor, ms.pData, sizeof(uint8_t) * 4);

			context->Unmap(mIndexStagingTexture.Get(), NULL);
		}
	}
}

/**
 * Creates the primary render target views:
 *  - Default back-buffer RTV.
 *  - Index RTV (via helper with additional textures).
 * Parameters:
 *  - device: D3D11 device used for resource creation.
 * Returns:
 *  - true if both RTVs created successfully; false on any failure.
 * Notes:
 *  - Relies on an existing swap chain (mSwapChain).
 */
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

/**
 * Creates the swap chain for this window.
 * Parameters:
 *  - renderer: Provides device and quality levels (MSAA).
 * Returns:
 *  - true on success; false if creation fails.
 * Preconditions:
 *  - mWinHandle must be a valid window handle.
 */
bool FTWindow::CreateSwapChain(FoxtrotRenderer* renderer)
{
	HRESULT hr = D3D11Utils::CreateSwapChain(mWinHandle, renderer->GetDevice(), mSwapChain, mWidth, mHeight, renderer->GetNumQualityLevels());
	if (hr != S_OK)
		return false;
	return true;
}

/**
 * Creates the depth stencil view (DSV) and associated depth buffer.
 * Parameters:
 *  - device: D3D11 device.
 *  - numQualityLevels: MSAA quality level count used to configure buffer.
 * Returns:
 *  - true if depth buffer creation succeeded; false otherwise.
 */
bool FTWindow::CreateDSV(ComPtr<ID3D11Device>& device, UINT numQualityLevels)
{
	HRESULT hr = D3D11Utils::CreateDepthBuffer(device, mWidth, mHeight, numQualityLevels, mDSV);
	if (hr != S_OK)
		return false;
	return true;
}

/**
 * Creates textures and RTV specifically used for index/color sampling:
 *  - mIndexTexture: Primary render target for index/color data.
 *  - mIndexTempTexture: Resolve target (multisample resolve source/dest).
 *  - mIndexStagingTexture: 1x1 staging texture for CPU read-back.
 * Parameters:
 *  - device: D3D11 device.
 * Returns:
 *  - true if all resources created successfully; false otherwise.
 * Notes:
 *  - Staging texture reduced to 1x1 to minimize read-back bandwidth.
 *  - Format: DXGI_FORMAT_R8G8B8A8_UNORM for simplicity and direct byte access.
 */
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

	// Creating 1x1 sized staging texture for lightweight CPU read-back.
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

// Accessors return references to internal COM pointers / window handle.
// Caller should not release these directly; lifetime managed by FTWindow.
HWND&							FTWindow::GetHandle() { return mWinHandle; }
ComPtr<IDXGISwapChain>&			FTWindow::GetSwapChain() { return mSwapChain; }
ComPtr<ID3D11RenderTargetView>& FTWindow::GetRTV() { return mRTV; }
ComPtr<ID3D11DepthStencilView>& FTWindow::GetDSV() { return mDSV; }
ComPtr<ID3D11Texture2D>&		FTWindow::GetIndexTexture() { return mIndexTexture; }
ComPtr<ID3D11Texture2D>&		FTWindow::GetIndexTempTexture() { return mIndexTempTexture; }
ComPtr<ID3D11Texture2D>&		FTWindow::GetIndexStagingTexture() { return mIndexStagingTexture; }
ComPtr<ID3D11RenderTargetView>& FTWindow::GetIndexRTV() { return mIndexRTV; }

FTRectArea*	   FTWindow::GetRenderArea() { return mRenderArea; }
FTInputDevice* FTWindow::GetInputDevice() { return mInputDevice; }

UINT FTWindow::GetWidth() { return mWidth; }
UINT FTWindow::GetHeight() { return mHeight; }

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

/**
 * Processes window messages and updates input device states.
 * Flow:
 *  - PeekMessage fetches pending message (non-blocking).
 *  - Passes message to mouse/key detectors.
 *  - Translates and dispatches the message.
 * Notes:
 *  - Only retrieves messages for this window handle.
 *  - Additional camera/editor input can be integrated where commented.
 */
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

/**
 * Resizes swap chain buffers and recreates dependent render targets and depth resources.
 * Parameters:
 *  - renderer: Provides device and MSAA quality information.
 * Behavior:
 *  - Releases old resources (Reset).
 *  - Calls IDXGISwapChain::ResizeBuffers with current mWidth/mHeight.
 *  - Recreates RTV and DSV.
 * Notes:
 *  - Caller must update any viewport/state using new dimensions afterward.
 */
void FTWindow::ResizeWindow(FoxtrotRenderer* renderer)
{
	Reset(renderer);
	if (mSwapChain)
	{
		DX::ThrowIfFailed(
			mSwapChain->ResizeBuffers(0, mWidth, mHeight, DXGI_FORMAT_UNKNOWN, 0));

		CreateRTV(renderer->GetDevice());
		CreateDSV(renderer->GetDevice(), renderer->GetNumQualityLevels());
		// renderer->SetViewport(0, 0, width, height);
	}
}

/**
 * Releases COM pointers for render targets and index sampling textures.
 * Parameters:
 *  - renderer: Currently unused; present for potential future logic.
 * Notes:
 *  - After Reset, resources must be recreated before rendering again.
 */
void FTWindow::Reset(FoxtrotRenderer* renderer)
{
	mRTV.Reset();
	mDSV.Reset();
	mIndexRTV.Reset();

	mIndexTexture.Reset();
	mIndexTempTexture.Reset();
	mIndexStagingTexture.Reset();
}

/**
 * Begins a frame render:
 *  - Clears RTVs and depth buffer.
 *  - Sets dual render targets (main + index).
 *  - Processes any pending window messages (non-blocking).
 *  - Updates OS window (InvalidateRect + UpdateWindow).
 *  - Sets viewport to current render area size.
 * Parameters:
 *  - renderer: Provides context, clear color, and viewport setup.
 * Performance:
 *  - InvalidateRect + UpdateWindow each frame may incur extra overhead; consider conditional invalidation.
 */
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
	renderer->SetViewport(0.f, 0.f, mRenderArea->GetSize().x, mRenderArea->GetSize().y);
}

/**
 * Ends a frame render:
 *  - Unbinds render targets and depth stencil state.
 * Parameters:
 *  - renderer: Provides device context.
 * Notes:
 *  - Does not present the swap chain; presentation likely handled externally.
 */
void FTWindow::EndRender(FoxtrotRenderer* renderer)
{
	ID3D11RenderTargetView* nullViews[] = { nullptr };
	renderer->GetContext()->OMSetRenderTargets(1, nullViews, nullptr);
	renderer->GetContext()->OMSetDepthStencilState(nullptr, 0);
}

/**
 * Clears all bound render targets and depth stencil view.
 * Parameters:
 *  - renderer: Provides context and clear color.
 * Notes:
 *  - Clear order: main RTV, index RTV, depth/stencil.
 */
void FTWindow::ClearWindow(FoxtrotRenderer* renderer)
{
	if (mRTV)
		renderer->GetContext()->ClearRenderTargetView(mRTV.Get(), renderer->GetClearColor());
	if (mIndexRTV)
		renderer->GetContext()->ClearRenderTargetView(mIndexRTV.Get(), renderer->GetClearColor());
	if (mDSV)
		renderer->GetContext()->ClearDepthStencilView(mDSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

/**
 * Determines if a given position lies inside the current render area.
 * Parameters:
 *  - pos: Position in window/client coordinates.
 * Returns:
 *  - true if pos overlaps mRenderArea; false otherwise.
 */
bool FTWindow::IsInRenderedArea(FTVector2 pos)
{
	return mRenderArea->Overlaps(pos);
}

/**
 * Constructor.
 * Parameters:
 *  - title: UTF-8 or ANSI string for window title (converted internally).
 *  - width / height: Desired client dimensions.
 *  - rndArea: Pointer to render area (ownership transferred; deleted in destructor).
 * Behavior:
 *  - Initializes input device.
 * Ownership:
 *  - Assumes rndArea allocated with new; destructor deletes it.
 */
FTWindow::FTWindow(const char* title, UINT width, UINT height, FTRectArea* rndArea)
	: mWinHandle(nullptr)
	, mWidth(width)
	, mHeight(height)
	, mRenderArea(rndArea)
	, mInputDevice(DBG_NEW FTInputDevice)
{
	mTitle.Assign(title);
}

/**
 * Destructor.
 * - Releases input device and render area (heap-allocated).
 * - Destroys the native window handle (if valid).
 * - COM resources should be previously released (Reset or resize path).
 */
FTWindow::~FTWindow()
{
	delete mInputDevice;
	delete mRenderArea;
	DestroyWindow(mWinHandle);
}