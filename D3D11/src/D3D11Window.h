#pragma once
#include "Renderer/FTWindow.h"

class D3D11Window :
	public FTWindow
{
public:
	// Constantly called in win32 MsgProc() function.
	virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

public:
	bool InitializeWindow(WNDPROC wndProc, int windowMode);
	bool InitializeWindow(WNDPROC wndProc);
	bool InitializeWindowRenderer(D3D11Renderer* renderer);
	bool CreateSwapChain(D3D11Renderer* renderer);

	void ResizeWindow(FoxtrotRenderer* renderer);

public:
	void ProcessInput(D3D11InputDevice* inputDevice);

	void BeginRender(FoxtrotRenderer* renderer) override;
	void EndRender(FoxtrotRenderer* renderer) override;

public:
	void Reset(FoxtrotRenderer* renderer);

public:
	// Accessors return references to internal COM pointers / window handle.
	// Caller should not release these directly; lifetime managed by FTWindow.
	HWND&											GetHandle();
	Microsoft::WRL::ComPtr<IDXGISwapChain>&			GetSwapChain();
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& GetRTV();
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>& GetDSV();

public:
	~D3D11Window() override;

private:
	HWND										   mWinHandle; ///< Native window handle.
	Microsoft::WRL::ComPtr<IDXGISwapChain>		   mSwapChain; ///< Swap chain for presenting frames.
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> mRTV;	   ///< Main render target view.
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> mDSV;	   ///< Depth-stencil view.

	FTInputDevice* mInputDevice;

private:
	/**
	 * @brief Creates and stores the main render target view from the swap chain back buffer.
	 * @param device D3D11 device.
	 * @return true on success.
	 */
	bool CreateRTV(Microsoft::WRL::ComPtr<ID3D11Device>& device);

	/**
	 * @brief Creates the depth-stencil buffer and view.
	 * @param device D3D11 device.
	 * @param numQualityLevels Multisample quality level to use (implementation-dependent).
	 * @return true on success.
	 */
	bool CreateDSV(Microsoft::WRL::ComPtr<ID3D11Device>& device, unsigned int numQualityLevels);

	void ClearWindow(D3D11Renderer* renderer);
};
