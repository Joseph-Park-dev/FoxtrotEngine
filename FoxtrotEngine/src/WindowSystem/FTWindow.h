#pragma once
#include <Windows.h>
#include <d3d11.h>
#include <string>
#include <wrl.h>

using namespace Microsoft::WRL;
class FoxtrotRenderer;
class FTVector2;
class FTRectArea;
class FTInputDevice;
enum class KEY;
enum class MOUSE;

class FTWindow
{
public:
	// This can be called anytime when you need a plain window.
	bool InitializeWindow(WNDPROC wndProc);

	// This should be called after the renderer is initialized.
	bool InitializeWindowRenderer(FoxtrotRenderer* renderer);
	// bool CreateSwapChain(ComPtr<ID3D11Device>& device);
	bool CreateSwapChain(FoxtrotRenderer* renderer);

	// Gets the pixel color at the cursor position,
	// pastes the value to mPickColor
	void SamplCursorPosColor(ComPtr<ID3D11DeviceContext>& context, uint8_t cursorPosColor[4]);

public:
	HWND&							GetHandle();
	ComPtr<IDXGISwapChain>&			GetSwapChain();
	ComPtr<ID3D11RenderTargetView>& GetRTV();
	ComPtr<ID3D11DepthStencilView>& GetDSV();
	ComPtr<ID3D11Texture2D>&		GetIndexTexture();
	ComPtr<ID3D11Texture2D>&		GetIndexTempTexture();
	ComPtr<ID3D11Texture2D>&		GetIndexStagingTexture();
	ComPtr<ID3D11RenderTargetView>& GetIndexRTV();

	UINT&		GetWidth();
	UINT&		GetHeight();
	FTRectArea* GetRenderArea();

	FTInputDevice* GetInputDevice();

	void SetWidth(UINT width);
	void SetHeight(UINT height);

public:
	bool KEY_HOLD(KEY key);
	bool KEY_TAP(KEY key);
	bool KEY_AWAY(KEY key);
	bool KEY_NONE(KEY key);

	bool MOUSE_HOLD(MOUSE mouse);
	bool MOUSE_TAP(MOUSE mouse);
	bool MOUSE_AWAY(MOUSE mouse);
	bool MOUSE_NONE(MOUSE mouse);

	FTVector2 MOUSE_POS();

public:
	void ProcessInput();
	void BeginRender(FoxtrotRenderer* renderer);
	void EndRender(FoxtrotRenderer* renderer);
	void ResizeWindow(FoxtrotRenderer* renderer);

	void Reset(FoxtrotRenderer* renderer);

public:
	FTWindow(const char* title, UINT width, UINT height);
	~FTWindow();

private:
	HWND						   mWinHandle;
	ComPtr<IDXGISwapChain>		   mSwapChain;
	ComPtr<ID3D11RenderTargetView> mRTV;
	ComPtr<ID3D11DepthStencilView> mDSV;

	// Indexing related (for mouse picking)
	ComPtr<ID3D11Texture2D>		   mIndexTexture;
	ComPtr<ID3D11Texture2D>		   mIndexTempTexture;
	ComPtr<ID3D11Texture2D>		   mIndexStagingTexture; // 1x1 sized
	ComPtr<ID3D11RenderTargetView> mIndexRTV;

	FTDS::String mTitle;
	UINT		 mWidth;
	UINT		 mHeight;

	FTInputDevice* mInputDevice;

	// The scene will be rendered on this portion of window.
	FTRectArea* mRenderArea;
	float		mClearColor[4];

private:
	void ClearWindow(FoxtrotRenderer* renderer);
	bool IsInRenderedArea(FTVector2 pos);

	bool CreateRTV(ComPtr<ID3D11Device>& device);
	bool CreateDSV(ComPtr<ID3D11Device>& device, UINT numQualityLevels);
	bool CreateIndexRTV(ComPtr<ID3D11Device>& device);
};