// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Renderer/FoxtrotRenderer.h"

#include <iostream>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <wrl.h> // ComPtr
#include <DirectXColors.h>
#include <directxtk/PrimitiveBatch.h>
#include <directxtk/VertexTypes.h>

#include "Core/TemplateFunctions.h"
#include "Actors/Transform.h"
#include "ResourceSystem/Vertex.h"
#include "Managers/KeyInputManager.h"
#include "Managers/SceneManager.h"
#include "Managers/DebugShapes.h"
#include "Renderer/D3D11Utils.h"
#include "Renderer/Camera.h"

#ifdef FOXTROT_EDITOR
	#define IMGUI_DEFINE_MATH_OPERATORS
	#include <imgui.h>
	#include "EditorLayer.h"
	#include "ViewportRenderer.h"
#endif // FOXTROT_EDITOR

FoxtrotRenderer* FoxtrotRenderer::CreateRenderer(HWND window, int width, int height)
{
	FoxtrotRenderer* ftRenderer = DBG_NEW FoxtrotRenderer();
	if (!ftRenderer->Initialize(window, width, height))
	{
		LogString("Failed to Initialize FTRenderer");
		return nullptr;
	}
	return ftRenderer;
}

void FoxtrotRenderer::DestroyRenderer(FoxtrotRenderer* renderer)
{
#ifdef FOXTROT_EDITOR
	// 렌더 텍스쳐 객체를 해제한다
	if (renderer->mViewportRenderer)
	{
		delete renderer->mViewportRenderer;
		renderer->mViewportRenderer = 0;
	}
#endif // FOXTROT_EDITOR

	if (renderer == nullptr)
	{
		LogString("Renderer is already null");
		return;
	}
	else
	{
		delete renderer;
		renderer = nullptr;
	}
}

void FoxtrotRenderer::SwapChainPresent(UINT syncInterval, UINT flags)
{
	mSwapChain->Present(syncInterval, flags);
}

ComPtr<ID3D11Device>&			FoxtrotRenderer::GetDevice() { return mDevice; }
ComPtr<ID3D11DeviceContext>&	FoxtrotRenderer::GetContext() { return mContext; }
ComPtr<IDXGISwapChain>&			FoxtrotRenderer::GetSwapChain() { return mSwapChain; }
ComPtr<ID3D11RenderTargetView>& FoxtrotRenderer::GetRenderTargetView() { return mRenderTargetView; }
ComPtr<ID3D11DepthStencilView>& FoxtrotRenderer::GetDSV() { return mDepthStencilView; }
ComPtr<ID3D11Texture2D>&		FoxtrotRenderer::GetDepthStencilBuffer() { return mDepthStencilBuffer; }
ComPtr<ID3D11BlendState>&		FoxtrotRenderer::GetBlendState() { return mBlendState; }

ComPtr<ID3D11Texture2D>&		FoxtrotRenderer::GetIndexTexture() { return mIndexTexture; }
ComPtr<ID3D11Texture2D>&		FoxtrotRenderer::GetIndexTempTexture() { return mIndexTempTexture; }
ComPtr<ID3D11Texture2D>&		FoxtrotRenderer::GetIndexStagingTexture() { return mIndexStagingTexture; }
ComPtr<ID3D11RenderTargetView>& FoxtrotRenderer::GetIndexRenderTargetView() { return mIndexRenderTargetView; }
uint8_t*						FoxtrotRenderer::GetCursorPosColor() { return mCursorPosColor; }

ComPtr<ID3D11DepthStencilState>& FoxtrotRenderer::GetDSS() { return mDepthStencilState; }
ComPtr<ID3D11DepthStencilState>& FoxtrotRenderer::GetDSS2D() { return mDepthStencilState2D; }

ComPtr<ID3D11VertexShader>& FoxtrotRenderer::GetSolidVS() { return mSolidVS; }
ComPtr<ID3D11InputLayout>&	FoxtrotRenderer::GetSolidInputLayout() { return mSolidInputLayout; }
ComPtr<ID3D11PixelShader>&	FoxtrotRenderer::GetSolidPS() { return mSolidPS; }

ComPtr<ID3D11VertexShader>& FoxtrotRenderer::GetTextureVS() { return mTextureVS; }
ComPtr<ID3D11InputLayout>&	FoxtrotRenderer::GetTextureInputLayout() { return mTextureInputLayout; }
ComPtr<ID3D11PixelShader>&	FoxtrotRenderer::GetTexturePS() { return mTexturePS; }
ComPtr<ID3D11PixelShader>&	FoxtrotRenderer::GetRimTexturePS() { return mRimTexturePS; }

ComPtr<ID3D11VertexShader>& FoxtrotRenderer::GetNormalVS() { return mNormalVS; }
ComPtr<ID3D11PixelShader>&	FoxtrotRenderer::GetNormalPS() { return mNormalPS; }

FTVector2 FoxtrotRenderer::GetRenderResolution()
{
	return FTVector2(static_cast<float>(mRenderWidth), static_cast<float>(mRenderHeight));
}

UINT FoxtrotRenderer::GetRenderWidth() const { return mRenderWidth; }
UINT FoxtrotRenderer::GetRenderHeight() const { return mRenderHeight; }
void FoxtrotRenderer::SetRenderWidth(const UINT width) { mRenderWidth = width; }
void FoxtrotRenderer::SetRenderHeight(const UINT height) { mRenderHeight = height; }
UINT FoxtrotRenderer::GetNumQualityLevels() { return mNumQualityLevels; }

void FoxtrotRenderer::RenderClear()
{
	float clearColor[4] = { 0.3f, 0.3f, 0.3f, 1.0 };
	mContext->ClearRenderTargetView(mRenderTargetView.Get(), clearColor);
	mContext->ClearRenderTargetView(mIndexRenderTargetView.Get(), clearColor);
	mContext->ClearDepthStencilView(mDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	mContext->OMSetDepthStencilState(mDepthStencilState.Get(), 0);
}

void FoxtrotRenderer::ResizeWindow(FTVector2& windowRes)
{
	mRenderTargetView.Reset();
	mDepthStencilView.Reset();
	mIndexRenderTargetView.Reset();
	if (mSwapChain)
	{
		DX::ThrowIfFailed(
			mSwapChain->ResizeBuffers(0, // 현재 개수 유지
									  static_cast<UINT>(windowRes.x),
									  static_cast<UINT>(windowRes.y),
									  DXGI_FORMAT_UNKNOWN, // 현재 포맷 유지
									  0));
		D3D11Utils::CreateRenderTargetView(mRenderTargetView, mDevice, mSwapChain);
		D3D11Utils::CreateDepthBuffer(mDevice, windowRes.x, windowRes.y, mNumQualityLevels, mDepthStencilView);

		// Viewport position & resolution.
#ifdef FOXTROT_EDITOR
		mViewportRenderer->Reset();
		ImVec2 topLeft = EditorLayer::GetInstance()->GetSceneViewportPos();
		mRenderWidth   = static_cast<int>(EditorLayer::GetInstance()->GetSceneViewportSize().x);
		mRenderHeight  = static_cast<int>(EditorLayer::GetInstance()->GetSceneViewportSize().y);
		// D3D11Utils::CreateRenderTargetView(mViewportRenderer->GetRTV(), mDevice, mSwapChain);
		mViewportRenderer->InitializeTexture(this);
#else
		FTVector2 topLeft = FTVector2(0.f, 0.f);
		mRenderWidth	  = static_cast<int>(windowRes.x);
		mRenderHeight	  = static_cast<int>(windowRes.y);
		D3D11Utils::CreateRenderTargetView(mIndexRenderTargetView, mDevice, mSwapChain, mIndexTexture, mIndexTempTexture, mIndexStagingTexture);
		D3D11Utils::CreateDepthBuffer(mDevice, windowRes.x, windowRes.y, mNumQualityLevels, mDepthStencilView);
#endif // FOXTROT_EDITOR

		SetViewport(0, 0, mRenderWidth, mRenderHeight);
	}
}

void FoxtrotRenderer::SampleCursorPosColor()
{
	// Copies the back buffer data to temp texture.
	if (mIndexTexture && mIndexTempTexture)
	{
		mContext->ResolveSubresource(mIndexTempTexture.Get(), 0, mIndexTexture.Get(), 0, DXGI_FORMAT_R8G8B8A8_UNORM);

		if (IsInRenderedArea(MOUSE_POS))
		{
			D3D11_BOX box;
			box.left   = MOUSE_POS.x;
			box.right  = MOUSE_POS.x + 1;
			box.top	   = MOUSE_POS.y;
			box.bottom = MOUSE_POS.y + 1;
			box.front  = 0;
			box.back   = 1;
			mContext->CopySubresourceRegion(mIndexStagingTexture.Get(), 0, 0, 0, 0, mIndexTempTexture.Get(), 0, &box);

			D3D11_MAPPED_SUBRESOURCE ms;
			mContext->Map(mIndexStagingTexture.Get(), NULL, D3D11_MAP_READ, NULL,
						  &ms); // D3D11_MAP_READ 주의

			if (ms.pData)
				memcpy(mCursorPosColor, ms.pData, sizeof(uint8_t) * 4);

			mContext->Unmap(mIndexStagingTexture.Get(), NULL);
		}
	}
}

void FoxtrotRenderer::SwitchFillMode() const
{
	if (mFillMode == FillMode::WireFrame)
		mContext->RSSetState(mWireframeRasterizerState.Get());
	else if (mFillMode == FillMode::Solid)
		mContext->RSSetState(mSolidRasterizerState.Get());
}

FillMode FoxtrotRenderer::GetFillMode() const { return mFillMode; }
void	 FoxtrotRenderer::SetFillMode(const FillMode mode) { mFillMode = mode; }

bool FoxtrotRenderer::Initialize(HWND window, int renderWidth, int renderHeight)
{
	mRenderWidth  = renderWidth;
	mRenderHeight = renderHeight;

	DX::ThrowIfFailed(D3D11Utils::CreateDeviceAndContext(
		window, mDevice, mContext, mSwapChain, mRenderWidth, mRenderHeight, mNumQualityLevels));

	/*HRESULT hr = CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);
	if (FAILED(hr))
		return false;*/

	DX::ThrowIfFailed(CreateRasterizerState());

	DX::ThrowIfFailed(D3D11Utils::CreateDepthBuffer(
		mDevice, mRenderWidth, mRenderHeight, mNumQualityLevels, mDepthStencilView));

	DX::ThrowIfFailed(CreateDepthStencilState(mDepthStencilState));
	DX::ThrowIfFailed(CreateDepthStencilState(mDepthStencilState2D, false));

	DX::ThrowIfFailed(CreateBlendState());
	mContext->OMSetBlendState(mBlendState.Get(), 0, D3D11_DEFAULT_SAMPLE_MASK);

	DX::ThrowIfFailed(CreateTextureSampler());

	DX::ThrowIfFailed(CoInitializeEx(nullptr, COINIT_MULTITHREADED));

	std::vector<D3D11_INPUT_ELEMENT_DESC> inputElements = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 4 * 3, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 4 * 3 + 4 * 3, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	std::vector<D3D11_INPUT_ELEMENT_DESC> inputElementsSolid = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 4 * 3, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 4 * 3 + 4 * 3, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 4 * 3 + 4 * 3 + 4 * 3, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	DX::ThrowIfFailed(
		D3D11Utils::CreateVertexShaderAndInputLayout(
			mDevice,
			SOLID_VS_PATH,
			inputElementsSolid,
			mSolidVS,
			mSolidInputLayout));

	DX::ThrowIfFailed(
		D3D11Utils::CreatePixelShader(
			mDevice,
			SOLID_PS_PATH,
			mSolidPS));

	DX::ThrowIfFailed(
		D3D11Utils::CreateVertexShaderAndInputLayout(
			mDevice,
			TEXTURE_VS_PATH,
			inputElements,
			mTextureVS,
			mTextureInputLayout));

	DX::ThrowIfFailed(
		D3D11Utils::CreatePixelShader(
			mDevice,
			TEXTURE_PS_PATH,
			mTexturePS));

	DX::ThrowIfFailed(
		D3D11Utils::CreatePixelShader(
			mDevice,
			RIM_TEXTURE_PS_PATH,
			mRimTexturePS));

	DX::ThrowIfFailed(
		D3D11Utils::CreateVertexShaderAndInputLayout(
			mDevice,
			NORMAL_VS_PATH,
			inputElements,
			mNormalVS,
			mTextureInputLayout));

	DX::ThrowIfFailed(
		D3D11Utils::CreatePixelShader(
			mDevice,
			NORMAL_PS_PATH,
			mNormalPS));

	mContext->OMSetDepthStencilState(mDepthStencilState.Get(), 0);

	mContext->VSSetShader(mSolidVS.Get(), 0, 0);

	mContext->PSSetSamplers(0, 1, mSamplerState.GetAddressOf());

	mContext->PSSetShader(mSolidPS.Get(), 0, 0);

	mContext->RSSetState(mSolidRasterizerState.Get());

	SetViewport(FTVector2(0.f, 0.f), FTVector2(mRenderWidth, mRenderHeight));

	DX::ThrowIfFailed(D3D11Utils::CreateRenderTargetView(mRenderTargetView, mDevice, mSwapChain));
#ifdef FOXTROT_EDITOR

#else
	D3D11Utils::CreateRenderTargetView(mIndexRenderTargetView, mDevice, mSwapChain, mIndexTexture, mIndexTempTexture, mIndexStagingTexture);
	ID3D11RenderTargetView* targets[] = { mRenderTargetView.Get(), mIndexRenderTargetView.Get() };
	mContext->OMSetRenderTargets(2, targets, mDepthStencilView.Get());
#endif //

#ifdef FOXTROT_EDITOR
	mViewportRenderer = DBG_NEW ViewportRenderer;
	if (!mViewportRenderer)
	{
		LogString("Error : FoxtrotRenderer Initialize - CreateRenderTexture failed.");
		return false;
	}
	mViewportRenderer->InitializeTexture(this);
#endif // FOXTROT_EDITOR
	return true;
}

HRESULT FoxtrotRenderer::CreateRasterizerState()
{
	// Create a rasterizer state
	D3D11_RASTERIZER_DESC rastDesc;
	ZeroMemory(&rastDesc, sizeof(D3D11_RASTERIZER_DESC)); // Need this
	// rastDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
	rastDesc.FillMode			   = D3D11_FILL_MODE::D3D11_FILL_SOLID;
	rastDesc.CullMode			   = D3D11_CULL_MODE::D3D11_CULL_NONE;
	rastDesc.FrontCounterClockwise = false;
	rastDesc.DepthClipEnable	   = true;

	HRESULT solidResult = mDevice->CreateRasterizerState(&rastDesc, &mSolidRasterizerState);

	rastDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;

	HRESULT wireResult = mDevice->CreateRasterizerState(&rastDesc, &mWireframeRasterizerState);

	return solidResult & wireResult;
}

void FoxtrotRenderer::SetViewport(FTVector2 topLeft, FTVector2 resolution)
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

void FoxtrotRenderer::SetViewport(FLOAT topLeftX, FLOAT topLeftY, FLOAT resX, FLOAT resY)
{
	// Set the viewport
	ZeroMemory(&mScreenViewport, sizeof(D3D11_VIEWPORT));

	mScreenViewport.TopLeftX = topLeftX;
	mScreenViewport.TopLeftY = topLeftY;
	mScreenViewport.Width	 = resX;
	mScreenViewport.Height	 = resY;
	// m_screenViewport.Width = static_cast<float>(m_screenHeight);
	mScreenViewport.MinDepth = 0.0f;
	mScreenViewport.MaxDepth = 1.0f; // Note: important for depth buffering
	mContext->RSSetViewports(1, &mScreenViewport);
}

HRESULT FoxtrotRenderer::CreateDepthStencilState(ComPtr<ID3D11DepthStencilState>& dss, bool depthEnabled)
{
	// Create depth stencil state
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	depthStencilDesc.DepthEnable	= depthEnabled; // false
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc		= D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;
	return mDevice->CreateDepthStencilState(&depthStencilDesc, dss.GetAddressOf());
}

HRESULT FoxtrotRenderer::CreateBlendState()
{
	D3D11_BLEND_DESC omDesc;
	ZeroMemory(&omDesc, sizeof(D3D11_BLEND_DESC));
	omDesc.RenderTarget[0].BlendEnable	  = true;
	omDesc.RenderTarget[0].SrcBlend		  = D3D11_BLEND_SRC_ALPHA;
	omDesc.RenderTarget[0].DestBlend	  = D3D11_BLEND_INV_SRC_ALPHA;
	omDesc.RenderTarget[0].BlendOp		  = D3D11_BLEND_OP_ADD;
	omDesc.RenderTarget[0].SrcBlendAlpha  = D3D11_BLEND_ONE;
	omDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	omDesc.RenderTarget[0].BlendOpAlpha	  = D3D11_BLEND_OP_ADD;
	omDesc.RenderTarget[0].RenderTargetWriteMask =
		D3D11_COLOR_WRITE_ENABLE_RED | D3D11_COLOR_WRITE_ENABLE_GREEN | D3D11_COLOR_WRITE_ENABLE_BLUE;

	return mDevice->CreateBlendState(&omDesc, mBlendState.GetAddressOf());
}

HRESULT FoxtrotRenderer::CreateTextureSampler()
{
	// FTTexture sampler 만들기
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter			= D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU		= D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressV		= D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressW		= D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD			= 0;
	sampDesc.MaxLOD			= D3D11_FLOAT32_MAX;

	// Create the Sample State
	return mDevice->CreateSamplerState(&sampDesc, mSamplerState.GetAddressOf());
}

bool FoxtrotRenderer::IsInRenderedArea(FTVector2 pos)
{
	return 0 <= pos.x && pos.x <= mRenderWidth - 1 &&
		0 <= pos.y && pos.y <= mRenderHeight - 1;
}

FoxtrotRenderer::FoxtrotRenderer()
	: mClearColor{ 0.0f, 0.0f, 0.0f, 1.0f }
	, mFillMode(FillMode::Solid)
#ifdef FOXTROT_EDITOR
	, mViewportRenderer(nullptr)
#endif // FOXTROT_EDITOR
{
}

#ifdef FOXTROT_EDITOR
void FoxtrotRenderer::RenderOnViewport()
{
	mViewportRenderer->DrawOnTexture(mContext, mRenderTargetView, mDepthStencilView, this);
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