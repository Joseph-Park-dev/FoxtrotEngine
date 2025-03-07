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
using namespace Microsoft::WRL;

class FoxtrotRenderer;

class ViewportRenderer
{
public:
	ViewportRenderer();
	~ViewportRenderer();

	void InitializeTexture(FoxtrotRenderer* renderer);

	// Switches the RenderTarget to this object's, renders the scene,
	// and switch RenderTarget back to FoxtrotRenderer's (in FTCoreEditor).
	void DrawOnTexture(
		ComPtr<ID3D11DeviceContext>&	context,
		ComPtr<ID3D11RenderTargetView>& renderTargetView,
		ComPtr<ID3D11DepthStencilView>& depthStencilView,
		FoxtrotRenderer*				renderer);

	void Reset();

public:
	ComPtr<ID3D11RenderTargetView>&	  GetViewportRTV() { return mViewportRTV; }
	ComPtr<ID3D11ShaderResourceView>& GetViewportSRV() { return mViewportSRV; }

private:
	ComPtr<ID3D11Texture2D>			 mViewportTex;
	ComPtr<ID3D11RenderTargetView>	 mViewportRTV;
	ComPtr<ID3D11ShaderResourceView> mViewportSRV;
	ComPtr<ID3D11DepthStencilView>	 mViewportDSV;

private:
	void CreateRenderTargetView(FoxtrotRenderer* renderer);
};
