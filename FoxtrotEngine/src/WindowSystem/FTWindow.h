// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include <Windows.h>
#include <d3d11.h>
#include <string>
#include <wrl.h>

class FoxtrotRenderer;
class FTVector2;
class FTRectArea;
class FTInputDevice;
enum class KEY;
enum class MOUSE;

/**
 * @class FTWindow
 * @brief Encapsulates a Win32/D3D11 renderable window, its swap chain, render/depth targets, and input handling.
 *
 * Responsibilities:
 *  - Creation and initialization of a native HWND window.
 *  - Integration with Direct3D 11 rendering (swap chain, RTV, DSV, indexing RTV for picking).
 *  - Delegation of input state queries via an internal FTInputDevice instance.
 *  - Providing a defined render sub-area inside the full window client area.
 *  - Handling per-frame render begin/end operations and resizing logic.
 *
 * Lifecycle:
 *  - Construct with title, dimensions, and render area.
 *  - Call InitializeWindow(...) first (before renderer init) to create the HWND.
 *  - After renderer/device creation, call InitializeWindowRenderer() / CreateSwapChain().
 *  - Use BeginRender()/EndRender() each frame.
 *  - On size changes, call ResizeWindow().
 *  - Call Reset() to fully recreate dependent D3D resources.
 *
 * Threading:
 *  - All methods assume calls from the main/UI thread that owns the window.
 *
 * Input:
 *  - KEY_* and MOUSE_* query transient and held states for this window's focused input device.
 *
 * Picking:
 *  - Index textures are used for object picking by rendering object IDs to a separate render target.
 */
class FTWindow
{
public:
	/**
	 * @brief Initializes the native window with a specific window mode (e.g., windowed, fullscreen).
	 * @param wndProc User-supplied window procedure.
	 * @param windowMode Mode flag controlling style/behavior (implementation-defined).
	 * @return true on success, false on failure.
	 */
	bool InitializeWindow(WNDPROC wndProc, int windowMode);

	/**
	 * @brief Initializes a plain/native window without specifying a mode (defaults applied).
	 * @param wndProc User-supplied window procedure.
	 * @return true on success, false otherwise.
	 */
	bool InitializeWindow(WNDPROC wndProc);

	/**
	 * @brief Completes window initialization that depends on an already-created renderer and D3D device.
	 * @param renderer Pointer to initialized FoxtrotRenderer.
	 * @return true if all D3D-related resources were created successfully.
	 */
	bool InitializeWindowRenderer(FoxtrotRenderer* renderer);

	/**
	 * @brief Creates the swap chain associated with this window using the renderer's device/factory.
	 * @param renderer Pointer to FoxtrotRenderer providing device and factory access.
	 * @return true on success, false otherwise.
	 */
	bool CreateSwapChain(FoxtrotRenderer* renderer);

	/**
	 * @brief Samples the pixel color at the current cursor position from the back buffer / staging resource.
	 * @param context Immediate or deferred device context to issue copy/read commands.
	 * @param cursorPosColor Output array of 4 bytes (RGBA) filled with sampled color.
	 * @note Requires prior setup of staging textures; may stall CPU if not optimized.
	 */
	void SamplCursorPosColor(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context, uint8_t cursorPosColor[4]);

public:
	/**
	 * @brief Gets the native window handle.
	 * @return Reference to HWND managed by this instance.
	 */
	HWND& GetHandle();

	/**
	 * @brief Accessor for the window's swap chain.
	 * @return Reference to IDXGISwapChain COM smart pointer.
	 */
	Microsoft::WRL::ComPtr<IDXGISwapChain>& GetSwapChain();

	/**
	 * @brief Accessor for the primary render target view.
	 * @return Reference to ID3D11RenderTargetView COM smart pointer.
	 */
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& GetRTV();

	/**
	 * @brief Accessor for the depth-stencil view.
	 * @return Reference to ID3D11DepthStencilView COM smart pointer.
	 */
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>& GetDSV();

	/**
	 * @brief Accessor for the picking/index main texture.
	 * @return Reference to ID3D11Texture2D COM smart pointer.
	 */
	Microsoft::WRL::ComPtr<ID3D11Texture2D>& GetIndexTexture();

	/**
	 * @brief Accessor for the temporary picking texture used during transfers.
	 * @return Reference to ID3D11Texture2D COM smart pointer.
	 */
	Microsoft::WRL::ComPtr<ID3D11Texture2D>& GetIndexTempTexture();

	/**
	 * @brief Accessor for the staging texture (1x1) used to read back a pixel.
	 * @return Reference to ID3D11Texture2D COM smart pointer.
	 */
	Microsoft::WRL::ComPtr<ID3D11Texture2D>& GetIndexStagingTexture();

	/**
	 * @brief Accessor for the render target view associated with the index texture.
	 * @return Reference to ID3D11RenderTargetView COM smart pointer.
	 */
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& GetIndexRTV();

	/**
	 * @brief Returns current window width in pixels (client area related).
	 * @return Unsigned width.
	 */
	UINT GetWidth();

	/**
	 * @brief Returns current window height in pixels (client area related).
	 * @return Unsigned height.
	 */
	UINT GetHeight();

	/**
	 * @brief Gets the render area (sub-rectangle) where the scene is drawn.
	 * @return Pointer to FTRectArea; may be smaller than full client size.
	 */
	FTRectArea* GetRenderArea();

	/**
	 * @brief Returns the input device associated with this window.
	 * @return Pointer to FTInputDevice.
	 */
	FTInputDevice* GetInputDevice();

	/**
	 * @brief Sets the window width (does not automatically resize swap chain).
	 * @param width New width value.
	 */
	void SetWidth(UINT width);

	/**
	 * @brief Sets the window height (does not automatically resize swap chain).
	 * @param height New height value.
	 */
	void SetHeight(UINT height);

public:
	/**
	 * @brief Checks if the specified key is currently held down (pressed and not yet released).
	 * @param key Enumerated key code.
	 * @return true if held.
	 */
	bool KEY_HOLD(KEY key);

	/**
	 * @brief Checks if the specified key was tapped (pressed this frame).
	 * @param key Enumerated key code.
	 * @return true if a press transition occurred this frame.
	 */
	bool KEY_TAP(KEY key);

	/**
	 * @brief Checks if the specified key was released (was down, now up) this frame.
	 * @param key Enumerated key code.
	 * @return true if a release transition occurred this frame.
	 */
	bool KEY_AWAY(KEY key);

	/**
	 * @brief Checks if the specified key is in neutral state (neither pressed nor transitioned).
	 * @param key Enumerated key code.
	 * @return true if no interaction state is active.
	 */
	bool KEY_NONE(KEY key);

	/**
	 * @brief Checks if a mouse button is currently held.
	 * @param mouse Enumerated mouse button.
	 * @return true if held.
	 */
	bool MOUSE_HOLD(MOUSE mouse);

	/**
	 * @brief Checks if a mouse button was tapped (pressed this frame).
	 * @param mouse Enumerated mouse button.
	 * @return true if transition to pressed occurred.
	 */
	bool MOUSE_TAP(MOUSE mouse);

	/**
	 * @brief Checks if a mouse button was released this frame.
	 * @param mouse Enumerated mouse button.
	 * @return true if transition to released occurred.
	 */
	bool MOUSE_AWAY(MOUSE mouse);

	/**
	 * @brief Checks if a mouse button is neutral (not held, no transition).
	 * @param mouse Enumerated mouse button.
	 * @return true if neutral.
	 */
	bool MOUSE_NONE(MOUSE mouse);

	/**
	 * @brief Returns current mouse cursor position in window client coordinates.
	 * @return FTVector2 representing (x,y).
	 */
	FTVector2 MOUSE_POS();

public:
	/**
	 * @brief Polls and processes input events, updating internal FTInputDevice states.
	 * @note Should be called once per frame before querying KEY_/MOUSE_ states.
	 */
	void ProcessInput();

	/**
	 * @brief Prepares render targets for a new frame (clears, sets RTV/DSV).
	 * @param renderer Active renderer instance.
	 */
	void BeginRender(FoxtrotRenderer* renderer);

	/**
	 * @brief Finalizes frame rendering (present swap chain, resolve, etc.).
	 * @param renderer Active renderer instance.
	 */
	void EndRender(FoxtrotRenderer* renderer);

	/**
	 * @brief Handles window resizing logic including swap chain buffer resizing and RTV/DSV recreation.
	 * @param renderer Active renderer instance.
	 */
	void ResizeWindow(FoxtrotRenderer* renderer);

	/**
	 * @brief Resets and recreates all D3D dependent resources (useful after device changes).
	 * @param renderer Active renderer instance.
	 */
	void Reset(FoxtrotRenderer* renderer);

public:
	/**
	 * @brief Constructs a window container with title and dimensions.
	 * @param title UTF-8 title string.
	 * @param width Initial window width.
	 * @param height Initial window height.
	 * @param rndArea Pointer to render area definition (may be identical to full window).
	 */
	FTWindow(const char* title, UINT width, UINT height, FTRectArea* rndArea);

	/**
	 * @brief Destructor releases COM resources and associated handles.
	 */
	~FTWindow();

private:
	HWND										   mWinHandle;              ///< Native window handle.
	Microsoft::WRL::ComPtr<IDXGISwapChain>		   mSwapChain;              ///< Swap chain for presenting frames.
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> mRTV;                    ///< Main render target view.
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> mDSV;                    ///< Depth-stencil view.

	// Indexing related (for mouse picking)
	Microsoft::WRL::ComPtr<ID3D11Texture2D>		   mIndexTexture;           ///< Texture storing per-pixel object/index IDs.
	Microsoft::WRL::ComPtr<ID3D11Texture2D>		   mIndexTempTexture;       ///< Intermediate texture for copy operations.
	Microsoft::WRL::ComPtr<ID3D11Texture2D>		   mIndexStagingTexture;    ///< 1x1 staging texture used for CPU readback.
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> mIndexRTV;               ///< RTV bound when rendering index/picking pass.

	FTDS::String mTitle;   ///< Window title string.

	UINT mWidth;           ///< Current window client width (not render area width).
	UINT mHeight;          ///< Current window client height (not render area height).

	FTInputDevice* mInputDevice; ///< Associated input device state manager.

	FTRectArea* mRenderArea;     ///< Sub-rectangle defining where the scene is drawn.

private:
	/**
	 * @brief Clears relevant render targets (color, depth, picking) at frame begin.
	 * @param renderer Active renderer instance.
	 */
	void ClearWindow(FoxtrotRenderer* renderer);

	/**
	 * @brief Determines if a position lies within the defined render area.
	 * @param pos Position in window client coordinates.
	 * @return true if inside render area.
	 */
	bool IsInRenderedArea(FTVector2 pos);

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
	bool CreateDSV(Microsoft::WRL::ComPtr<ID3D11Device>& device, UINT numQualityLevels);

	/**
	 * @brief Creates the render target view used for object index/picking pass.
	 * @param device D3D11 device.
	 * @return true on success.
	 */
	bool CreateIndexRTV(Microsoft::WRL::ComPtr<ID3D11Device>& device);
};