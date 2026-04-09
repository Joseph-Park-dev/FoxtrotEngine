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

#include "Renderer/D3D11Renderer.h"
#include "Renderer/D3D11Utils.h"
#include "Renderer/FTWindow.h"
#include "Renderer/FTRectArea.h"
#include "TemplateFunctions.h"
#include "Manager/DebugShapes.h"

#include "EditorLayer.h"
#include "EditorSceneManager.h"
#include "EditorChunkLoader.h"
#include "EditorCamera.h"

namespace Editor
{
	using namespace D3D11;
	void ViewportRenderer::InitializeTexture(D3D11Renderer* renderer, ImVec2 size)
	{
		if (0 < size.x && 0 < size.y)
			CreateRenderTargetView(renderer, size.x, size.y);
	}

	void ViewportRenderer::BeginRender(D3D11Renderer* renderer)
	{
		float clearColor[] = { 0.f, 0.f, 0.f, 0.f };
		if (mRTV)
			renderer->GetContext()
				->ClearRenderTargetView(mRTV.Get(), clearColor);
		if (mDSV)
			renderer->GetContext()->ClearDepthStencilView(mDSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		ID3D11RenderTargetView* targetsPrev[] = { mRTV.Get() };
		renderer->GetContext()->OMSetRenderTargets(1, targetsPrev, mDSV.Get());
	}

	void ViewportRenderer::DrawOnTexture(D3D11Renderer* renderer)
	{
		if (!EditorChunkLoader::GetInstance()->IsLoadingChunk())
		{
			EditorSceneManager::GetInstance()->EditorRender(renderer);
			DebugShapes::GetInstance()->Render(renderer);
			DebugShapes::GetInstance()->RenderCamRect(renderer);
			LightManager::GetInstance()->Render(renderer, EditorCamera::GetInstance());
		}
	}

	void ViewportRenderer::EndRender(D3D11Renderer* renderer)
	{
		ID3D11RenderTargetView* nullViews[] = { nullptr };
		renderer->GetContext()->OMSetRenderTargets(1, nullViews, nullptr);
		renderer->GetContext()->OMSetDepthStencilState(nullptr, 0);
	}

	void ViewportRenderer::Reset()
	{
		mRenderTexture.Reset();
		mRTV.Reset();
		mSRV.Reset();
		mDSV.Reset();
	}

	void ViewportRenderer::CreateRenderTargetView(D3D11Renderer* renderer, UINT width, UINT height)
	{
		Reset();

		////////////////////////////////////////////
		////// Creating Viewport RenderTarget //////
		////////////////////////////////////////////

		// Initialize Texture2D
		D3D11_TEXTURE2D_DESC textureDesc;
		ZeroMemory(&textureDesc, sizeof(textureDesc));

		textureDesc.Width			 = width;
		textureDesc.Height			 = height;
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
			D3D11Utils::CreateDepthBuffer(renderer->GetDevice(), width, height, renderer->GetNumQualityLevels(), mDSV));

		renderer->GetDevice()->CreateShaderResourceView(mRenderTexture.Get(), 0, mSRV.GetAddressOf());
	}

	ViewportRenderer::ViewportRenderer()
		: mRenderTexture(nullptr)
		, mRTV(nullptr)
		, mSRV(nullptr)
	{
	}

	ViewportRenderer::~ViewportRenderer()
	{
	}
} // namespace Editor