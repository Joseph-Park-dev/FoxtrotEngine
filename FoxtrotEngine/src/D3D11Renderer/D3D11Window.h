#pragma once
#include <Core/Renderer/FTWindow.h>

class D3D11Window :
	public FTWindow
{
private:
	HWND										   mWinHandle; ///< Native window handle.
	Microsoft::WRL::ComPtr<IDXGISwapChain>		   mSwapChain; ///< Swap chain for presenting frames.
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> mRTV;	   ///< Main render target view.
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> mDSV;	   ///< Depth-stencil view.

	// Indexing related (for mouse picking)
	Microsoft::WRL::ComPtr<ID3D11Texture2D>		   mIndexTexture;		 ///< Texture storing per-pixel object/index IDs.
	Microsoft::WRL::ComPtr<ID3D11Texture2D>		   mIndexTempTexture;	 ///< Intermediate texture for copy operations.
	Microsoft::WRL::ComPtr<ID3D11Texture2D>		   mIndexStagingTexture; ///< 1x1 staging texture used for CPU readback.
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> mIndexRTV;			 ///< RTV bound when rendering index/picking pass.
};
