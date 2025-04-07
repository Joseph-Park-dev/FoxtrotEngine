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
#include "WindowSystem/FTWindow.h"
#include "Renderer/FTRectArea.h"
#include "Core/TemplateFunctions.h"
#include "Physics/Physics2D.h"
#include "Managers/DebugShapes.h"

#include "EditorLayer.h"
#include "EditorSceneManager.h"
#include "EditorChunkLoader.h"

void ViewportRenderer::InitializeTexture(FTWindow* window, FoxtrotRenderer* renderer, UINT width, UINT height)
{
	ImVec2 pos = EditorLayer::GetInstance()->GetSceneViewportPos();
	window->GetRenderArea()->Set(pos.x, pos.y, static_cast<float>(width), static_cast<float>(height));
	CreateRenderTargetView(window, renderer);
}

void ViewportRenderer::BeginRender(FoxtrotRenderer* renderer)
{
	float test[4] = { 0.3f,0.3f,0.3f,1.0f };
	if (mRTV)
		renderer->GetContext()->ClearRenderTargetView(mRTV.Get(), test);
	if (mDSV)
		renderer->GetContext()->ClearDepthStencilView(mDSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	ID3D11RenderTargetView* targetsPrev[] = { mRTV.Get() };
	renderer->GetContext()->OMSetRenderTargets(1, targetsPrev, mDSV.Get());
}

void ViewportRenderer::DrawOnTexture(FoxtrotRenderer* renderer)
{
	if (!EditorChunkLoader::GetInstance()->IsLoadingChunk())
	{
		//EditorSceneManager::GetInstance()->Render(renderer);
		EditorSceneManager::GetInstance()->EditorRender(renderer);
		DebugShapes::GetInstance()->Render(renderer);
	}
}

void ViewportRenderer::EndRender(FoxtrotRenderer* renderer)
{
	ID3D11RenderTargetView* nullViews[] = { nullptr };
	renderer->GetContext()->OMSetRenderTargets(1, nullViews, nullptr);
	renderer->GetContext()->OMSetDepthStencilState(nullptr, 0);
}

void ViewportRenderer::Resize(FoxtrotRenderer* renderer)
{
	Reset();
	ImVec2 topLeft = EditorLayer::GetInstance()->GetSceneViewportPos();
	mWidth = static_cast<int>(EditorLayer::GetInstance()->GetSceneViewportSize().x);
	mHeight = static_cast<int>(EditorLayer::GetInstance()->GetSceneViewportSize().y);
	//InitializeTexture( renderer, mWidth, mHeight);
}

void ViewportRenderer::Reset()
{
	mRenderTexture.Reset();
	mRTV.Reset();
	mSRV.Reset();
	mDSV.Reset();
}

void ViewportRenderer::CreateRenderTargetView(
	FTWindow* window, FoxtrotRenderer* renderer)
{
	Reset();

	////////////////////////////////////////////
	////// Creating Viewport RenderTarget //////
	////////////////////////////////////////////

	// Initialize Texture2D
	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(textureDesc));

	FTRectArea* renderArea = window->GetRenderArea();
	FTVector2 renderSize = renderArea->GetSize();

	textureDesc.Width			 = renderSize.x;
	textureDesc.Height			 = renderSize.y;
	textureDesc.MipLevels		 = 1;
	textureDesc.ArraySize		 = 1;
	textureDesc.Format			 = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage			 = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags		 = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags	 = 0;
	textureDesc.MiscFlags		 = 0;

	DX::ThrowIfFailed(renderer->GetDevice()->CreateTexture2D(&textureDesc, NULL, mRenderTexture.GetAddressOf()));

	// Initialize RenderTargetView
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	renderTargetViewDesc.Format				= textureDesc.Format;
	renderTargetViewDesc.ViewDimension		= D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	DX::ThrowIfFailed(
		renderer->GetDevice()->CreateRenderTargetView(
			mRenderTexture.Get(),
			&renderTargetViewDesc,
			mRTV.GetAddressOf()));

	DX::ThrowIfFailed(
		D3D11Utils::CreateDepthBuffer(renderer->GetDevice(), renderSize.x, renderSize.y, renderer->GetNumQualityLevels(), mDSV));

	renderer->GetDevice()->CreateShaderResourceView(mRenderTexture.Get(), 0, mSRV.GetAddressOf());
}

ViewportRenderer::ViewportRenderer()
	: mRenderTexture(nullptr)
	, mRTV(nullptr)
	, mSRV(nullptr)
	, mWidth(0)
	, mHeight(0)
{
}

ViewportRenderer::~ViewportRenderer()
{
}