// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Displays viewport as a RenderTexture, showing the Scene.
/// </summary>

#pragma once
#include "Plugin/D3D11Exports.h"
#include "Plugin/D3D11Exports.h"
#include <wrl/client.h>
#include <d3d11.h>


namespace D3D11
{
	class FTRectArea;
} // namespace D3D11

namespace D3D11
{
	class D3D11Renderer;
	class ViewportRenderer
	{
	public:
		/// @brief Creates the viewport or resource texture and associated views.
		/// @param renderer Renderer providing the graphics device and current render state.
		/// @param xSize Horizontal extent.
		/// @param ySize Vertical extent.
		D3D11_API void InitializeTexture(D3D11::D3D11Renderer* renderer, float xSize, float ySize);

		/// @brief Binds and clears the render target and configures the viewport for a rendering pass.
		/// @param renderer Renderer providing the graphics device and current render state.
		D3D11_API void BeginRender(D3D11::D3D11Renderer* renderer);
		// Switches the RenderTarget to this object's, renders the scene,
		// and switch RenderTarget back to FoxtrotRenderer's (in FTCoreEditor).
		/// @brief Provides an empty lifecycle or extension hook for this implementation.
		/// @param renderer Renderer providing the graphics device and current render state.
		D3D11_API void DrawOnTexture(D3D11::D3D11Renderer* renderer);
		/// @brief Finishes the rendering pass and restores or presents its target.
		/// @param renderer Renderer providing the graphics device and current render state.
		D3D11_API void EndRender(D3D11::D3D11Renderer* renderer);

		/// @brief Clears transient state before the next processing cycle.
		D3D11_API void Reset();

	public:
		/// @brief Returns the viewport rtv used by this viewport renderer.
		/// @return Borrowed access to the viewport rtv.
		/// @note Changes through the returned reference affect this object's stored state.
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView>&	  GetViewportRTV() { return mRTV; }
		/// @brief Returns the viewport srv used by this viewport renderer.
		/// @return Borrowed access to the viewport srv.
		/// @note Changes through the returned reference affect this object's stored state.
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& GetViewportSRV() { return mSRV; }

	public:
		/// @brief Initializes off-screen texture and render-target state.
		D3D11_API ViewportRenderer();
		/// @brief Completes destruction through the object's inheritance hierarchy.
		D3D11_API ~ViewportRenderer();

	private:
		Microsoft::WRL::ComPtr<ID3D11Texture2D>			 mRenderTexture;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView>	 mRTV;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mSRV;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView>	 mDSV;

	private:
		/// @brief Creates a render-target view used to bind a texture for rendering.
		/// @param renderer Renderer providing the graphics device and current render state.
		/// @param width Width of the window, texture, or geometry.
		/// @param height Height of the window, texture, or geometry.
		/// @throws D3D11::DX::com_exception If a checked Direct3D operation fails.
		D3D11_API void CreateRenderTargetView(D3D11::D3D11Renderer* renderer, UINT width, UINT height);
	};

	/// @brief Creates an off-screen renderer for a scene viewport.
	/// @return Created viewport renderer instance or resource.
	extern "C" D3D11_API ViewportRenderer* CreateViewportRenderer();
    /// @brief Releases a viewport renderer and its associated GPU resources.
    /// @param renderer Renderer providing the graphics device and current render state.
    extern "C" D3D11_API void DestroyViewportRenderer(ViewportRenderer* renderer);
    /// @brief Updates the editor gui context used by subsequent operations.
    /// @param context Replacement editor gui context.
    extern "C" D3D11_API void SetEditorGuiContext(void* context);
} // namespace D3D11
