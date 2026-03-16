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

class FTWindow
{
public:
	bool InitializeWindow(WNDPROC wndProc, int windowMode);
	bool InitializeWindow(WNDPROC wndProc);
	bool InitializeWindowRenderer(FoxtrotRenderer* renderer);
	bool CreateSwapChain(FoxtrotRenderer* renderer);
	void SamplCursorPosColor(Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context, uint8_t cursorPosColor[4]);

public:
	HWND&											GetHandle();
	Microsoft::WRL::ComPtr<IDXGISwapChain>&			GetSwapChain();
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& GetRTV();
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>& GetDSV();
	Microsoft::WRL::ComPtr<ID3D11Texture2D>&		GetIndexTexture();
	Microsoft::WRL::ComPtr<ID3D11Texture2D>&		GetIndexTempTexture();
	Microsoft::WRL::ComPtr<ID3D11Texture2D>&		GetIndexStagingTexture();
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& GetIndexRTV();
	UINT											GetWidth();
	UINT											GetHeight();
	FTRectArea*										GetRenderArea();
	FTInputDevice*									GetInputDevice();

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
	FTDS::String mTitle; ///< Window title string.

	UINT mWidth;  ///< Current window client width (not render area width).
	UINT mHeight; ///< Current window client height (not render area height).

	FTInputDevice* mInputDevice; ///< Associated input device state manager.

	FTRectArea* mRenderArea; ///< Sub-rectangle defining where the scene is drawn.

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