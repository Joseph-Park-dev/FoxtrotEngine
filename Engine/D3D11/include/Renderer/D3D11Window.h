#pragma once
#include "Plugin/D3D11Exports.h"
#include "Renderer/IWindow.h"

#include <Windows.h>
#include <wrl.h>
#include <d3d11.h>

namespace Graphics
{
	class IRenderer;
} // namespace Graphics

namespace D3D11
{
	class D3D11InputDevice;
	class D3D11Renderer;
	class D3D11Window;

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
		public Graphics::IWindow
	{
	public:
		D3D11_API void BeginRender(Graphics::IRenderer* renderer) override;
		D3D11_API void EndRender(Graphics::IRenderer* renderer) override;
		D3D11_API void ResizeWindow(Graphics::IRenderer* renderer) override;

	public:
		D3D11_API bool Initialize(int windowMode);
		D3D11_API bool Initialize(int windowMode, WNDPROC proc, WNDPROC_Params* params = nullptr);
		D3D11_API bool InitializeWindowRenderer(Graphics::IRenderer* renderer);
		D3D11_API bool CreateSwapChain(Graphics::IRenderer* renderer);

	public:
		D3D11_API void Reset();
        void* NativeHandle() const override { return mWinHandle; }

	public:
		// Accessors return references to internal COM pointers / window handle.
		// Caller should not release these directly; lifetime managed by FTWindow.
		D3D11_API HWND&											GetHandle();
		D3D11_API Microsoft::WRL::ComPtr<IDXGISwapChain>&			GetSwapChain();
		D3D11_API Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& GetRTV();
		D3D11_API Microsoft::WRL::ComPtr<ID3D11DepthStencilView>& GetDSV();

		//////////////////////////////////////////
		////// Window Properties /////////////////
		//////////////////////////////////////////

		/// <summary>
		/// Gets the window title.
		/// </summary>
		D3D11_API virtual const char* GetTitle() const override;

		/// <summary>
		/// Gets the window client width in pixels.
		/// </summary>
		D3D11_API virtual unsigned int GetWidth() const override;

		/// <summary>
		/// Gets the window client height in pixels.
		/// </summary>
		D3D11_API virtual unsigned int GetHeight() const override;

		/// <summary>
		/// Sets the window client width.
		/// </summary>
		D3D11_API virtual void SetWidth(unsigned int width) override;

		/// <summary>
		/// Sets the window client height.
		/// </summary>
		D3D11_API virtual void SetHeight(unsigned int height) override;

		/// <summary>
		/// Gets the render area rectangle within the window.
		/// </summary>
		D3D11_API virtual D3D11::FTRectArea* GetRenderArea() const override;

	public:
		D3D11_API D3D11Window(const char* title, unsigned int width, unsigned int height, D3D11::FTRectArea* rndArea);
		D3D11_API D3D11Window(const char* title, unsigned int width, unsigned int height, D3D11::FTRectArea* rndArea, WNDPROC proc, WNDPROC_Params* params);
		D3D11_API ~D3D11Window() override;

	private:
		HWND										   mWinHandle; ///< Native window handle.
		Microsoft::WRL::ComPtr<IDXGISwapChain>		   mSwapChain; ///< Swap chain for presenting frames.
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> mRTV;	   ///< Main render target view.
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> mDSV;	   ///< Depth-stencil view.

		Common::FTDS::String* mTitle;
		unsigned int		  mWidth;
		unsigned int		  mHeight;
		D3D11::FTRectArea*	  mRenderArea;

	private:
		/**
		 * @brief Creates and stores the main render target view from the swap chain back buffer.
		 * @param device D3D11 device.
		 * @return true on success.
		 */
		D3D11_API bool CreateRTV(Microsoft::WRL::ComPtr<ID3D11Device>& device);

		/**
		 * @brief Creates the depth-stencil buffer and view.
		 * @param device D3D11 device.
		 * @param numQualityLevels Multisample quality level to use (implementation-dependent).
		 * @return true on success.
		 */
		D3D11_API bool CreateDSV(Microsoft::WRL::ComPtr<ID3D11Device>& device, unsigned int numQualityLevels);

		D3D11_API void ClearWindow(D3D11Renderer* renderer);

		D3D11_API static LRESULT CALLBACK WinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	};
} // namespace D3D11