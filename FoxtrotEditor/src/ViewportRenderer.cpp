// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "ViewportRenderer.h"

#include <d3d11.h>
#include <wrl.h>
#include <functional>

#include "Renderer/FoxtrotRenderer.h"
#include "Renderer/D3D11Utils.h"
#include "Core/TemplateFunctions.h"
#include "Physics/Physics2D.h"
#include "Managers/DebugShapes.h"

#include "EditorLayer.h"
#include "EditorSceneManager.h"
#include "EditorChunkLoader.h"

void ViewportRenderer::InitializeTexture(FoxtrotRenderer* renderer, int width, int height, UINT numQualityLevels)
{
	CreateRenderTargetView(renderer);
}

void ViewportRenderer::DrawOnTexture(ComPtr<ID3D11DeviceContext>& context, ComPtr<ID3D11RenderTargetView>& renderTargetView, ComPtr<ID3D11DepthStencilView>& depthStencilView, FoxtrotRenderer* renderer)
{
	float clearColor[4] = { 0.3, 0.3, 0.3, 1.0 };
	context->ClearRenderTargetView(mViewportRTV.Get(), clearColor);
	context->ClearRenderTargetView(renderer->GetIndexRenderTargetView().Get(), clearColor);
	context->ClearDepthStencilView(mViewportDSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	ID3D11RenderTargetView* targetsPrev[] = { mViewportRTV.Get(),
											  renderer->GetIndexRenderTargetView().Get() };
	context->OMSetRenderTargets(2, targetsPrev, mViewportDSV.Get());

	if (!EditorChunkLoader::GetInstance()->IsLoadingChunk())
	{
		EditorSceneManager::GetInstance()->Render(renderer);
		EditorSceneManager::GetInstance()->EditorRender(renderer);
		DebugShapes::GetInstance()->Render(renderer);
		renderer->SampleCursorPosColor();
	}

	ID3D11RenderTargetView* targetsAfter[] = { renderTargetView.Get() };
	context->OMSetRenderTargets(1, targetsAfter, depthStencilView.Get());
}

void ViewportRenderer::Reset()
{
	mViewportTex.Reset();
	mViewportRTV.Reset();
	mViewportSRV.Reset();
	mViewportDSV.Reset();
}

void ViewportRenderer::CreateRenderTargetView(
	FoxtrotRenderer* renderer)
{
	mViewportRTV.Reset();
	mViewportSRV.Reset();
	mViewportDSV.Reset();

	////////////////////////////////////////////
	////// Creating Viewport RenderTarget //////
	////////////////////////////////////////////

	// Initialize Texture2D
	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(textureDesc));

	textureDesc.Width			 = renderer->GetRenderWidth();
	textureDesc.Height			 = renderer->GetRenderHeight();
	textureDesc.MipLevels		 = 1;
	textureDesc.ArraySize		 = 1;
	textureDesc.Format			 = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage			 = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags		 = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags	 = 0;
	textureDesc.MiscFlags		 = 0;

	DX::ThrowIfFailed(renderer->GetDevice()->CreateTexture2D(&textureDesc, NULL, mViewportTex.GetAddressOf()));

	// Initialize RenderTargetView
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	renderTargetViewDesc.Format				= textureDesc.Format;
	renderTargetViewDesc.ViewDimension		= D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	DX::ThrowIfFailed(
		renderer->GetDevice()->CreateRenderTargetView(
			mViewportTex.Get(),
			&renderTargetViewDesc,
			mViewportRTV.GetAddressOf()));

	DX::ThrowIfFailed(
		D3D11Utils::CreateDepthBuffer(renderer->GetDevice(), renderer->GetRenderWidth(), renderer->GetRenderHeight(), renderer->GetNumQualityLevels(), mViewportDSV));

	renderer->GetDevice()->CreateShaderResourceView(mViewportTex.Get(), 0, mViewportSRV.GetAddressOf());

	//////////////////////////////////////////
	////// Creating Index Render Target //////
	//////////////////////////////////////////

	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	DX::ThrowIfFailed(renderer->GetDevice()->CreateTexture2D(&textureDesc, NULL, renderer->GetIndexTempTexture().GetAddressOf()));
	DX::ThrowIfFailed(renderer->GetDevice()->CreateTexture2D(&textureDesc, NULL, renderer->GetIndexTexture().GetAddressOf()));

	// Creating 1x1 sized staging texture
	textureDesc.BindFlags	   = 0;
	textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	textureDesc.Usage		   = D3D11_USAGE_STAGING;
	textureDesc.Width		   = 1;
	textureDesc.Height		   = 1;

	DX::ThrowIfFailed(renderer->GetDevice()->CreateTexture2D(
		&textureDesc, nullptr, renderer->GetIndexStagingTexture().GetAddressOf()));

	DX::ThrowIfFailed(
		renderer->GetDevice()->CreateRenderTargetView(
			renderer->GetIndexTexture().Get(),
			&renderTargetViewDesc,
			renderer->GetIndexRenderTargetView().GetAddressOf()));
}

ViewportRenderer::ViewportRenderer()
	: mViewportTex(nullptr)
	, mViewportRTV(nullptr)
	, mViewportSRV(nullptr)
{
}

ViewportRenderer::~ViewportRenderer()
{
}