// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Renderer/FoxtrotRenderer.h"

#include "TemplateFunctions.h"
#include "Actor/Transform.h"
#include "InputSystem/FTInputDevice.h"
#include "Manager/SceneManager.h"
#include "Renderer/Camera.h"
#include "Renderer/FTRectArea.h"
#include "Renderer/FTWindow.h"

#ifdef FOXTROT_EDITOR
	#define IMGUI_DEFINE_MATH_OPERATORS
	#include <imgui.h>
	#include "EditorLayer.h"
	#include "ViewportRenderer.h"

	#include "EditorChunkLoader.h"
	#include "EditorSceneManager.h"
	#include "FoxtrotRenderer.h"
#endif // FOXTROT_EDITOR

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

FoxtrotRenderer::FoxtrotRenderer(FTWindow* window)
	: mClearColor{ 0.3f, 0.3f, 0.3f, 1.0f }
#ifdef FOXTROT_EDITOR
	, mViewportRenderer(DBG_NEW ViewportRenderer)
#endif // FOXTROT_EDITOR
{
}

FoxtrotRenderer::~FoxtrotRenderer()
{
}

#ifdef FOXTROT_EDITOR
bool FoxtrotRenderer::InitializeViewport(FTWindow* window, UINT posX, UINT posY, UINT width, UINT height)
{
	if (!mViewportRenderer)
	{
		LogString("Error : FoxtrotRenderer Initialize - CreateRenderTexture failed.");
		return false;
	}
	ImVec2 pos;
	pos.x = static_cast<float>(posX);
	pos.y = static_cast<float>(posY);

	ImVec2 size;
	size.x = static_cast<float>(width);
	size.y = static_cast<float>(height);
	mViewportRenderer->InitializeTexture(this, size);
	return true;
}

void FoxtrotRenderer::RenderOnViewport()
{
	mViewportRenderer->DrawOnTexture(this);
}

void FoxtrotRenderer::SetViewport(const ImVec2& topLeft, const ImVec2& resolution)
{
	// Set the viewport
	ZeroMemory(&mScreenViewport, sizeof(D3D11_VIEWPORT));

	mScreenViewport.TopLeftX = topLeft.x;
	mScreenViewport.TopLeftY = topLeft.y;
	mScreenViewport.Width	 = resolution.x;
	mScreenViewport.Height	 = resolution.y;
	// m_screenViewport.Width = static_cast<float>(m_screenHeight);
	mScreenViewport.MinDepth = 0.0f;
	mScreenViewport.MaxDepth = 1.0f; // Note: important for depth buffering
	mContext->RSSetViewports(1, &mScreenViewport);
}

ViewportRenderer* FoxtrotRenderer::GetViewportRenderer()
{
	return mViewportRenderer;
}
#endif // FOXTROT_EDITOR