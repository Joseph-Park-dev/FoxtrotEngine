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
		/// @brief Initializes the context forwarded to a native window procedure.
		/// @param win Window used by the operation.
		/// @param input Input state for the current frame.
		/// @param rend Renderer providing the graphics backend.
		/// @param isResizing Whether a resize operation is active.
		/// @note Initializes the Window base or delegates to its constructor.
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
		/// @brief Binds and clears the render target and configures the viewport for a rendering pass.
		/// @param renderer Renderer providing the graphics device and current render state.
		/// @throws D3D11::DX::com_exception If a checked Direct3D operation fails.
		D3D11_API void BeginRender(Graphics::IRenderer* renderer) override;
		/// @brief Finishes the rendering pass and restores or presents its target.
		/// @param renderer Renderer providing the graphics device and current render state.
		D3D11_API void EndRender(Graphics::IRenderer* renderer) override;
		/// @brief Updates the window presentation resources to match its client dimensions.
		/// @param renderer Renderer providing the graphics device and current render state.
		/// @throws D3D11::DX::com_exception If a checked Direct3D operation fails.
		D3D11_API void ResizeWindow(Graphics::IRenderer* renderer) override;

	public:
		/// @brief Initializes the services and state required before this object's runtime lifecycle begins.
		/// @param windowMode Window presentation mode.
		/// @return True if the operation succeeds or the tested condition holds; otherwise false.
		D3D11_API bool Initialize(int windowMode);
		/// @brief Initializes the services and state required before this object's runtime lifecycle begins.
		/// @param windowMode Window presentation mode.
		/// @param proc Exported function or window procedure.
		/// @param params Opaque parameters forwarded to the callback.
		/// @return True if the operation succeeds or the tested condition holds; otherwise false.
		D3D11_API bool Initialize(int windowMode, WNDPROC proc, WNDPROC_Params* params = nullptr);
		/// @brief Initializes the rendering resources associated with a native window.
		/// @param renderer Renderer providing the graphics device and current render state.
		/// @return True if the operation succeeds or the tested condition holds; otherwise false.
		D3D11_API bool InitializeWindowRenderer(Graphics::IRenderer* renderer);
		/// @brief Creates the swap chain used to present rendered frames in the native window.
		/// @param renderer Renderer providing the graphics device and current render state.
		/// @return True if the operation succeeds or the tested condition holds; otherwise false.
		D3D11_API bool CreateSwapChain(Graphics::IRenderer* renderer);

	public:
		/// @brief Clears transient state before the next processing cycle.
		D3D11_API void Reset();
        /// @brief Exposes the underlying platform window handle.
        /// @return The underlying platform window handle.
        void* NativeHandle() const override { return mWinHandle; }

	public:
		// Accessors return references to internal COM pointers / window handle.
		// Caller should not release these directly; lifetime managed by FTWindow.
		/// @brief Returns the handle used by this d3 d11 window.
		/// @return Borrowed access to the handle.
		/// @note Changes through the returned reference affect this object's stored state.
		D3D11_API HWND&											GetHandle();
		/// @brief Returns the swap chain used by this d3 d11 window.
		/// @return Borrowed access to the swap chain.
		/// @note Changes through the returned reference affect this object's stored state.
		D3D11_API Microsoft::WRL::ComPtr<IDXGISwapChain>&			GetSwapChain();
		/// @brief Returns the rtv used by this d3 d11 window.
		/// @return Borrowed access to the rtv.
		/// @note Changes through the returned reference affect this object's stored state.
		D3D11_API Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& GetRTV();
		/// @brief Returns the dsv used by this d3 d11 window.
		/// @return Borrowed access to the dsv.
		/// @note Changes through the returned reference affect this object's stored state.
		D3D11_API Microsoft::WRL::ComPtr<ID3D11DepthStencilView>& GetDSV();

		//////////////////////////////////////////
		////// Window Properties /////////////////
		//////////////////////////////////////////

		/// @brief Returns the title used by this d3 d11 window.
		/// Gets the window title.
		/// @return Borrowed access to the title.
		D3D11_API virtual const char* GetTitle() const override;

		/// @brief Returns the width used by this d3 d11 window.
		/// Gets the window client width in pixels.
		/// @return Current width.
		D3D11_API virtual unsigned int GetWidth() const override;

		/// @brief Returns the height used by this d3 d11 window.
		/// Gets the window client height in pixels.
		/// @return Current height.
		D3D11_API virtual unsigned int GetHeight() const override;

		/// @brief Updates the width used by subsequent operations.
		/// Sets the window client width.
		/// @param width Replacement width.
		D3D11_API virtual void SetWidth(unsigned int width) override;

		/// @brief Updates the height used by subsequent operations.
		/// Sets the window client height.
		/// @param height Replacement height.
		D3D11_API virtual void SetHeight(unsigned int height) override;

		/// @brief Returns the render area used by this d3 d11 window.
		/// Gets the render area rectangle within the window.
		/// @return Borrowed access to the render area.
		D3D11_API virtual D3D11::FTRectArea* GetRenderArea() const override;

	public:
		/// @brief Initializes native window and presentation state.
		/// @param title Window title or dialog caption.
		/// @param width Width of the window, texture, or geometry.
		/// @param height Height of the window, texture, or geometry.
		/// @param rndArea Area used for rendering.
		D3D11_API D3D11Window(const char* title, unsigned int width, unsigned int height, D3D11::FTRectArea* rndArea);
		/// @brief Initializes native window and presentation state.
		/// @param title Window title or dialog caption.
		/// @param width Width of the window, texture, or geometry.
		/// @param height Height of the window, texture, or geometry.
		/// @param rndArea Area used for rendering.
		/// @param proc Exported function or window procedure.
		/// @param params Opaque parameters forwarded to the callback.
		D3D11_API D3D11Window(const char* title, unsigned int width, unsigned int height, D3D11::FTRectArea* rndArea, WNDPROC proc, WNDPROC_Params* params);
		/// @brief Releases the resources managed by this instance during destruction.
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
		/// @brief Creates and stores the main render target view from the swap chain back buffer.
		/// @param device D3D11 device.
		/// @return true on success.
		D3D11_API bool CreateRTV(Microsoft::WRL::ComPtr<ID3D11Device>& device);

		/// @brief Creates the depth-stencil buffer and view.
		/// @param device D3D11 device.
		/// @param numQualityLevels Multisample quality level to use (implementation-dependent).
		/// @return true on success.
		D3D11_API bool CreateDSV(Microsoft::WRL::ComPtr<ID3D11Device>& device, unsigned int numQualityLevels);

		/// @brief Clears the window's render target for the next frame.
		/// @param renderer Renderer providing the graphics device and current render state.
		D3D11_API void ClearWindow(D3D11Renderer* renderer);

		/// @brief Dispatches native window messages for input and window lifecycle handling.
		/// @param hwnd Native window receiving the message.
		/// @param msg Windows message containing input or window data.
		/// @param wParam Message-specific Windows parameter.
		/// @param lParam Message-specific Windows parameter.
		/// @return Dispatches native window messages for input and window lifecycle handling.
		D3D11_API static LRESULT CALLBACK WinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	};
} // namespace D3D11
