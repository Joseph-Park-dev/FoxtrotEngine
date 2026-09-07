#include <imgui.h>
// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Renderer/ViewportRenderer.h"

#include <d3d11.h>
#include <wrl.h>
#include <functional>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>

#include "Renderer/D3D11Renderer.h"
#include "Utility/D3D11Utils.h"
#include "Debugging/D3D11DebugFuncs.h"
#include "Renderer/IWindow.h"
#include "Renderer/FTRectArea.h"
#include "TemplateFunctions.h"
#include "Manager/DebugShapes.h"

namespace D3D11
{
	void ViewportRenderer::InitializeTexture(D3D11Renderer* renderer, float xSize, float ySize)
	{
		// Quantize UI dimensions to whole pixels only within the D3D11 texture limit.
        // These comparisons also reject NaN and infinity before conversion to UINT.
        if (1 <= xSize && xSize <= D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION &&
            1 <= ySize && ySize <= D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION)
            CreateRenderTargetView(renderer, static_cast<UINT>(xSize), static_cast<UINT>(ySize));
	}

	void ViewportRenderer::BeginRender(D3D11Renderer* renderer)
	{
        if (mRenderTexture) {
            D3D11_TEXTURE2D_DESC desc{};
            mRenderTexture->GetDesc(&desc);
            renderer->SetViewport(0, 0, static_cast<float>(desc.Width), static_cast<float>(desc.Height));
        }
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
		// if (!EditorChunkLoader::GetInstance()->IsLoadingChunk())
		//{
		//  EditorSceneManager::GetInstance()->EditorRender(renderer);
		//  DebugShapes::GetInstance()->Render(renderer);
		//  DebugShapes::GetInstance()->RenderCamRect(renderer);
		//  LightManager::GetInstance()->Render(renderer, EditorCamera::GetInstance());
		//}
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

	D3D11_API ViewportRenderer* CreateViewportRenderer()
	{
		return DBG_NEW ViewportRenderer;
	}
} // namespace D3D11
namespace D3D11 {
void DestroyViewportRenderer(ViewportRenderer* renderer) { delete renderer; }
void SetEditorGuiContext(void* context) { ImGui::SetCurrentContext(static_cast<ImGuiContext*>(context)); }
}

D3D11::ViewportRenderer::~ViewportRenderer() = default;
