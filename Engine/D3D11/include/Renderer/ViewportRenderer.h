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
		D3D11_API void InitializeTexture(D3D11::D3D11Renderer* renderer, float xSize, float ySize);

		D3D11_API void BeginRender(D3D11::D3D11Renderer* renderer);
		// Switches the RenderTarget to this object's, renders the scene,
		// and switch RenderTarget back to FoxtrotRenderer's (in FTCoreEditor).
		D3D11_API void DrawOnTexture(D3D11::D3D11Renderer* renderer);
		D3D11_API void EndRender(D3D11::D3D11Renderer* renderer);

		D3D11_API void Reset();

	public:
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView>&	  GetViewportRTV() { return mRTV; }
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& GetViewportSRV() { return mSRV; }

	public:
		D3D11_API ViewportRenderer();
		D3D11_API ~ViewportRenderer();

	private:
		Microsoft::WRL::ComPtr<ID3D11Texture2D>			 mRenderTexture;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView>	 mRTV;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mSRV;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView>	 mDSV;

	private:
		D3D11_API void CreateRenderTargetView(D3D11::D3D11Renderer* renderer, UINT width, UINT height);
	};

	extern "C" D3D11_API ViewportRenderer* CreateViewportRenderer();
    extern "C" D3D11_API void DestroyViewportRenderer(ViewportRenderer* renderer);
    extern "C" D3D11_API void SetEditorGuiContext(void* context);
} // namespace D3D11