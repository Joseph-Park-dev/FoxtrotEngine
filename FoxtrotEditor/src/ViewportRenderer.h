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

class FTWindow;
class FoxtrotRenderer;

class ViewportRenderer
{
public:
	void InitializeTexture(FTWindow* window, FoxtrotRenderer* renderer, UINT width, UINT height);

	// Switches the RenderTarget to this object's, renders the scene,
	// and switch RenderTarget back to FoxtrotRenderer's (in FTCoreEditor).
	void DrawOnTexture(FoxtrotRenderer* renderer);

	void Resize(FoxtrotRenderer* renderer);
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

	UINT mWidth;
	UINT mHeight;

private:
	void CreateRenderTargetView(FTWindow* window, FoxtrotRenderer* renderer);
};
