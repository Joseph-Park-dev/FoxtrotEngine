#pragma once
#include "Renderer/IWindow.h"

#include <Windows.h>
#include <wrl.h>
#include <d3d11.h>

namespace Core
{
	class FTCore;
	class IPlugin;
	class IRenderer;
} // namespace Core

namespace D3D11
{
	class D3D11InputDevice;
	class D3D11Renderer;

	struct WNDPROC_Params
	{
		WNDPROC_Params(D3D11::D3D11Window* win, D3D11::D3D11InputDevice* input, D3D11::D3D11Renderer* rend, bool* isResizing)
			: Window(win)
			, InputDevice(input)
			, Renderer(rend)
			, IsResizingWin(isResizing) {}

		D3D11::D3D11Window*		 Window;
		D3D11::D3D11InputDevice* InputDevice;
		D3D11::D3D11Renderer*	 Renderer;
		bool*					 IsResizingWin;
	};

	class D3D11Window :
		public Core::IWindow
	{
	public:
		void BeginRender(Core::IRenderer* renderer) override;
		void EndRender(Core::IRenderer* renderer) override;
		void ResizeWindow(Core::IRenderer* renderer) override;

	public:
		bool Initialize(int windowMode);
		bool Initialize(int windowMode, WNDPROC proc, WNDPROC_Params* params = nullptr);
		bool InitializeWindowRenderer(D3D11::D3D11Renderer* renderer);
		bool CreateSwapChain(D3D11::D3D11Renderer* renderer);

	public:
		void Reset();

	public:
		// Accessors return references to internal COM pointers / window handle.
		// Caller should not release these directly; lifetime managed by FTWindow.
		HWND&											GetHandle();
		Microsoft::WRL::ComPtr<IDXGISwapChain>&			GetSwapChain();
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& GetRTV();
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView>& GetDSV();

		//////////////////////////////////////////
		////// Window Properties /////////////////
		//////////////////////////////////////////

		/// <summary>
		/// Gets the window title.
		/// </summary>
		virtual const Common::FTDS::String* GetTitle() const override;

		/// <summary>
		/// Gets the window client width in pixels.
		/// </summary>
		virtual unsigned int GetWidth() const override;

		/// <summary>
		/// Gets the window client height in pixels.
		/// </summary>
		virtual unsigned int GetHeight() const override;

		/// <summary>
		/// Sets the window client width.
		/// </summary>
		virtual void SetWidth(unsigned int width) override;

		/// <summary>
		/// Sets the window client height.
		/// </summary>
		virtual void SetHeight(unsigned int height) override;

		/// <summary>
		/// Gets the render area rectangle within the window.
		/// </summary>
		virtual FTRectArea* GetRenderArea() const override;

	public:
		D3D11Window(const char* title, unsigned int width, unsigned int height, Core::FTRectArea* rndArea);
		D3D11Window(const char* title, unsigned int width, unsigned int height, Core::FTRectArea* rndArea, WNDPROC proc, WNDPROC_Params* params);
		~D3D11Window() override;

	private:
		HWND										   mWinHandle; ///< Native window handle.
		Microsoft::WRL::ComPtr<IDXGISwapChain>		   mSwapChain; ///< Swap chain for presenting frames.
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> mRTV;	   ///< Main render target view.
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> mDSV;	   ///< Depth-stencil view.

		Common::FTDS::String* mTitle;
		unsigned int		  mWidth;
		unsigned int		  mHeight;
		FTRectArea*			  mRenderArea;

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