// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Renderer/FoxtrotRenderer.h"

// void FoxtrotRenderer::SwapChainPresent(UINT syncInterval, UINT flags)
//{
//	mSwapChain->Present(syncInterval, flags);
// }

// bool FoxtrotRenderer::Initialize()
//{
//	// DX::ThrowIfFailed(D3D11Utils::CreateDeviceAndContext(
//	//	window->GetHandle(), mDevice, mContext, window->GetSwapChain(), renderWidth, renderHeight, mNumQualityLevels));
//
//	///*HRESULT hr = CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);
//	// if (FAILED(hr))
//	//	return false;*/
//
//	// DX::ThrowIfFailed(CreateRasterizerState());
//
//	// DX::ThrowIfFailed(CreateDepthStencilState(mDepthStencilState));
//	// DX::ThrowIfFailed(CreateDepthStencilState(mDepthStencilState2D, false));
//
//	// DX::ThrowIfFailed(CreateBlendState());
//	// FLOAT blendFactor[4] = { 1.f, 1.f, 1.f, 1.f };
//	// mContext->OMSetBlendState(mBlendState.Get(), blendFactor, D3D11_DEFAULT_SAMPLE_MASK);
//
//	// DX::ThrowIfFailed(CreateTextureSampler());
//
//	// DX::ThrowIfFailed(CoInitializeEx(nullptr, COINIT_MULTITHREADED));
//
//	// mContext->OMSetDepthStencilState(mDepthStencilState.Get(), 0);
//
//	// mContext->VSSetShader(mSolidVS.Get(), 0, 0);
//
//	// mContext->PSSetSamplers(0, 1, mSamplerState.GetAddressOf());
//
//	// mContext->PSSetShader(mSolidPS.Get(), 0, 0);
//
//	// mContext->RSSetState(mSolidRasterizerState.Get());
//
//	return true;
// }