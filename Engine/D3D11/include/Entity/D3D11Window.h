#pragma once
#include "Renderer/FTWindow.h"

#include <Windows.h>
#include <wrl.h>
#include <d3d11.h>

namespace Core
{
	class FTCore;
	class IPlugin;
} // namespace Core

namespace D3D11
{
	class D3D11InputDevice;
	class D3D11Renderer;
	class D3D11Window;

	struct WNDPROC_Params
	{
		WNDPROC_Params(Core::FTCore* core, D3D11::D3D11Window* win, D3D11::D3D11InputDevice* input, D3D11::D3D11Renderer* rend, bool* isResizing)
			: Core(core)
			, Window(win)
			, InputDevice(input)
			, Renderer(rend)
			, IsResizingWin(isResizing) {}

		Core::FTCore*			 Core;
		D3D11::D3D11Window*		 Window;
		D3D11::D3D11InputDevice* InputDevice;
		D3D11::D3D11Renderer*	 Renderer;
		bool*					 IsResizingWin;
	};

	class D3D11Window :
		public Core::FTWindow
	{
	public:
		bool Initialize(int windowMode);
		bool Initialize(int windowMode, WNDPROC proc, WNDPROC_Params* params = nullptr);
		bool InitializeWindowRenderer(D3D11Renderer* renderer);
		bool CreateSwapChain(D3D11Renderer* renderer);

		void ResizeWindow(Core::FoxtrotRenderer* renderer) override;

	public:
		void ProcessInput(D3D11InputDevice* inputDevice);
		void BeginRender(Core::FoxtrotRenderer* renderer) override;
		void EndRender(Core::FoxtrotRenderer* renderer) override;

	public:
		void Reset(Core::FoxtrotRenderer* renderer);

	public:
		// Accessors return references to internal COM pointers / window handle.
		// Caller should not release these directly; lifetime managed by FTWindow.
		HWND&											GetHandle();
		Microsoft::WRL::ComPtr<IDXGISwapChain>&			GetSwapChain();
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& GetRTV();
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView>& GetDSV();

	public:
		D3D11Window(const char* title, unsigned int width, unsigned int height, Core::FTRectArea* rndArea);
		D3D11Window(const char* title, unsigned int width, unsigned int height, Core::FTRectArea* rndArea, WNDPROC proc, WNDPROC_Params* params);
		~D3D11Window() override;

	private:
		HWND										   mWinHandle; ///< Native window handle.
		Microsoft::WRL::ComPtr<IDXGISwapChain>		   mSwapChain; ///< Swap chain for presenting frames.
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> mRTV;	   ///< Main render target view.
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> mDSV;	   ///< Depth-stencil view.

		Core::FTInputDevice* mInputDevice;

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

		static LRESULT WinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	};
} // namespace D3D11