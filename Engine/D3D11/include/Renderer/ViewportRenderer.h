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

namespace Core
{
	class IWindow;
} // namespace Core

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
		void InitializeTexture(D3D11::D3D11Renderer* renderer, float xSize, float ySize);

		void BeginRender(D3D11::D3D11Renderer* renderer);
		// Switches the RenderTarget to this object's, renders the scene,
		// and switch RenderTarget back to FoxtrotRenderer's (in FTCoreEditor).
		void DrawOnTexture(D3D11::D3D11Renderer* renderer);
		void EndRender(D3D11::D3D11Renderer* renderer);

		void Reset();

	public:
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView>&	  GetViewportRTV() { return mRTV; }
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& GetViewportSRV() { return mSRV; }

	public:
		ViewportRenderer();

	private:
		Microsoft::WRL::ComPtr<ID3D11Texture2D>			 mRenderTexture;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView>	 mRTV;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mSRV;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView>	 mDSV;

	private:
		void CreateRenderTargetView(D3D11::D3D11Renderer* renderer, UINT width, UINT height);
	};

	extern "C" D3D11_API ViewportRenderer* CreateViewportRenderer();
} // namespace D3D11