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
#include <d3d11.h>
#include <wrl.h> // ComPtr
#include <imgui/imgui.h>

using namespace Microsoft::WRL;

namespace Core
{
	class FTWindow;
	class FTRectArea;
} // namespace Core

namespace D3D11
{
	class D3D11Renderer;
}

namespace Editor
{
	class ViewportRenderer
	{
	public:
		void InitializeTexture(D3D11::D3D11Renderer* renderer, ImVec2 size);

		void BeginRender(D3D11::D3D11Renderer* renderer);
		// Switches the RenderTarget to this object's, renders the scene,
		// and switch RenderTarget back to FoxtrotRenderer's (in FTCoreEditor).
		void DrawOnTexture(D3D11::D3D11Renderer* renderer);
		void EndRender(D3D11::D3D11Renderer* renderer);

		void Reset();

	public:
		ComPtr<ID3D11RenderTargetView>&	  GetViewportRTV() { return mRTV; }
		ComPtr<ID3D11ShaderResourceView>& GetViewportSRV() { return mSRV; }

	public:
		ViewportRenderer();
		~ViewportRenderer();

	private:
		ComPtr<ID3D11Texture2D>			 mRenderTexture;
		ComPtr<ID3D11RenderTargetView>	 mRTV;
		ComPtr<ID3D11ShaderResourceView> mSRV;
		ComPtr<ID3D11DepthStencilView>	 mDSV;

	private:
		void CreateRenderTargetView(D3D11::D3D11Renderer* renderer, UINT width, UINT height);
	};
} // namespace Editor