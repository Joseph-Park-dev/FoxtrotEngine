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
#include "InputSystem/FTInputDevice.h"
#include "Managers/SceneManager.h"
#include "Managers/DebugShapes.h"
#include "Renderer/D3D11Utils.h"
#include "Renderer/Camera.h"
#include "WindowSystem/FTWindow.h"

#ifdef FOXTROT_EDITOR
	#define IMGUI_DEFINE_MATH_OPERATORS
	#include <imgui.h>
	#include "EditorLayer.h"
	#include "ViewportRenderer.h"

	#include "EditorChunkLoader.h"
	#include "EditorSceneManager.h"
#endif // FOXTROT_EDITOR

FoxtrotRenderer* FoxtrotRenderer::CreateRenderer(FTWindow* window, int width, int height)
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
		renderer->mViewportRenderer = nullptr;
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

// void FoxtrotRenderer::SwapChainPresent(UINT syncInterval, UINT flags)
//{
//	mSwapChain->Present(syncInterval, flags);
// }

ComPtr<ID3D11Device>&		 FoxtrotRenderer::GetDevice() { return mDevice; }
ComPtr<ID3D11DeviceContext>& FoxtrotRenderer::GetContext() { return mContext; }
ComPtr<ID3D11BlendState>&	 FoxtrotRenderer::GetBlendState() { return mBlendState; }

ComPtr<ID3D11DepthStencilState>& FoxtrotRenderer::GetDSS() { return mDepthStencilState; }
ComPtr<ID3D11DepthStencilState>& FoxtrotRenderer::GetDSS2D() { return mDepthStencilState2D; }

ComPtr<ID3D11VertexShader>& FoxtrotRenderer::GetSolidVS() { return mSolidVS; }
ComPtr<ID3D11InputLayout>&	FoxtrotRenderer::GetSolidInputLayout() { return mSolidInputLayout; }
ComPtr<ID3D11PixelShader>&	FoxtrotRenderer::GetSolidPS() { return mSolidPS; }

ComPtr<ID3D11VertexShader>& FoxtrotRenderer::GetTextureVS() { return mTextureVS; }
ComPtr<ID3D11InputLayout>&	FoxtrotRenderer::GetTextureInputLayout() { return mTextureInputLayout; }
ComPtr<ID3D11PixelShader>&	FoxtrotRenderer::GetTexturePS() { return mTexturePS; }
ComPtr<ID3D11PixelShader>&	FoxtrotRenderer::GetTexture2DPS() { return mTexture2DPS; }
ComPtr<ID3D11PixelShader>&	FoxtrotRenderer::GetRimTexturePS() { return mRimTexturePS; }

ComPtr<ID3D11VertexShader>& FoxtrotRenderer::GetNormalVS() { return mNormalVS; }
ComPtr<ID3D11PixelShader>&	FoxtrotRenderer::GetNormalPS() { return mNormalPS; }

ComPtr<ID3D11VertexShader>& FoxtrotRenderer::GetCubeMapVS() { return mCubeMapVS; }
ComPtr<ID3D11PixelShader>&	FoxtrotRenderer::GetCubeMapPS() { return mCubeMapPS; }

const float* FoxtrotRenderer::GetClearColor() const
{
	return mClearColor;
}

UINT FoxtrotRenderer::GetNumQualityLevels() { return mNumQualityLevels; }

uint8_t* FoxtrotRenderer::GetCursorPosColor()
{
	return mCursorPosColor;
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

bool FoxtrotRenderer::Initialize(FTWindow* window, int renderWidth, int renderHeight)
{
	DX::ThrowIfFailed(D3D11Utils::CreateDeviceAndContext(
		window->GetHandle(), mDevice, mContext, window->GetSwapChain(), renderWidth, renderHeight, mNumQualityLevels));

	/*HRESULT hr = CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);
	if (FAILED(hr))
		return false;*/

	DX::ThrowIfFailed(CreateRasterizerState());

	DX::ThrowIfFailed(CreateDepthStencilState(mDepthStencilState));
	DX::ThrowIfFailed(CreateDepthStencilState(mDepthStencilState2D, false));

	DX::ThrowIfFailed(CreateBlendState());
	FLOAT blendFactor[4] = { 1.f, 1.f, 1.f, 1.f };
	mContext->OMSetBlendState(mBlendState.Get(), blendFactor, D3D11_DEFAULT_SAMPLE_MASK);

	DX::ThrowIfFailed(CreateTextureSampler());

	DX::ThrowIfFailed(CoInitializeEx(nullptr, COINIT_MULTITHREADED));

	mContext->OMSetDepthStencilState(mDepthStencilState.Get(), 0);

	mContext->VSSetShader(mSolidVS.Get(), 0, 0);

	mContext->PSSetSamplers(0, 1, mSamplerState.GetAddressOf());

	mContext->PSSetShader(mSolidPS.Get(), 0, 0);

	mContext->RSSetState(mSolidRasterizerState.Get());

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
	if (mContext)
		mContext->RSSetViewports(1, &mScreenViewport);
}

void FoxtrotRenderer::Reset()
{
	mSolidRasterizerState.Reset();
	mWireframeRasterizerState.Reset();
	mDepthStencilState.Reset();
	mDepthStencilState2D.Reset();
	mSamplerState.Reset();
	mSolidVS.Reset();
	mSolidPS.Reset();
	mSolidInputLayout.Reset();
	mTextureVS.Reset();
	mTexturePS.Reset();
	mRimTexturePS.Reset();
	mTextureInputLayout.Reset();
	mNormalVS.Reset();
	mNormalPS.Reset();
	mBlendState.Reset();
	mContext->ClearState();

#ifdef FOXTROT_EDITOR
	mViewportRenderer->Reset();
#endif // FOXTROT_EDITOR
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
	D3D11_BLEND_DESC blendDesc		 = {};
	ZeroMemory(&blendDesc, sizeof(D3D11_BLEND_DESC));
	blendDesc.AlphaToCoverageEnable	 = FALSE;
	blendDesc.IndependentBlendEnable = FALSE;

	D3D11_RENDER_TARGET_BLEND_DESC& rtBlendDesc = blendDesc.RenderTarget[0];
	rtBlendDesc.BlendEnable						= TRUE;
	rtBlendDesc.SrcBlend						= D3D11_BLEND_ONE;
	rtBlendDesc.DestBlend						= D3D11_BLEND_INV_SRC_ALPHA;
	rtBlendDesc.BlendOp							= D3D11_BLEND_OP_ADD;
	rtBlendDesc.SrcBlendAlpha					= D3D11_BLEND_ONE;
	rtBlendDesc.DestBlendAlpha					= D3D11_BLEND_INV_SRC_ALPHA;
	rtBlendDesc.BlendOpAlpha					= D3D11_BLEND_OP_ADD;
	rtBlendDesc.RenderTargetWriteMask			= D3D11_COLOR_WRITE_ENABLE_ALL;

	return mDevice->CreateBlendState(&blendDesc, mBlendState.GetAddressOf());
}

HRESULT FoxtrotRenderer::CreateTextureSampler()
{
	// FTTexture sampler 만들기
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter			= D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU		= D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV		= D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW		= D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD			= 0;
	sampDesc.MaxLOD			= D3D11_FLOAT32_MAX;

	// Create the Sample State
	return mDevice->CreateSamplerState(&sampDesc, mSamplerState.GetAddressOf());
}

FoxtrotRenderer::FoxtrotRenderer()
	: mClearColor{ 0.3f, 0.3f, 0.3f, 1.0f }
	, mFillMode(FillMode::Solid)
#ifdef FOXTROT_EDITOR
	, mViewportRenderer(DBG_NEW ViewportRenderer)
#endif // FOXTROT_EDITOR
{
}

FoxtrotRenderer::~FoxtrotRenderer()
{
}

#ifdef FOXTROT_EDITOR
bool FoxtrotRenderer::InitializeViewport(FTWindow* window, UINT renderWidth, UINT renderHeight)
{
	if (!mViewportRenderer)
	{
		LogString("Error : FoxtrotRenderer Initialize - CreateRenderTexture failed.");
		return false;
	}
	mViewportRenderer->InitializeTexture(window, this, renderWidth, renderHeight);
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