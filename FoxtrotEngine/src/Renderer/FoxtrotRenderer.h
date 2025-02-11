// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Main renderer from FoxtrotEngine.
/// Takes care of the rendering operations in DirectX 11 context.
/// </summary>

#pragma once
#pragma comment(lib, "D3DCompiler.lib")
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <directxtk/SimpleMath.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <wrl.h> // ComPtr
#include <iostream>
#include <exception>

#include <DirectXColors.h>
#include <directxtk/PrimitiveBatch.h>
#include <directxtk/VertexTypes.h>

#include "ResourceSystem/Mesh.h"
#include "Core/TemplateFunctions.h"
#include "Debugging/DebugMemAlloc.h"

#define SOLID_VS_PATH L"Assets/Shaders/SolidVS.hlsl"
#define SOLID_PS_PATH L"Assets/Shaders/SolidPS.hlsl"
#define TEXTURE_VS_PATH L"Assets/Shaders/TextureVS.hlsl"
#define TEXTURE_PS_PATH L"Assets/Shaders/TexturePS.hlsl"
#define BLINN_PHONG_PS_PATH L"Assets/Shaders/BlinnPhongPS.hlsl"

using VertexType = DirectX::VertexPositionColor;
using namespace Microsoft::WRL;
using namespace DX;

class Transform;
class RenderTextureClass;
class FTVector2;

enum class FillMode
{
	WireFrame,
	Solid
};

class FoxtrotRenderer
{
public:
	// Intended to be used during intialization.
	static FoxtrotRenderer* CreateRenderer(HWND window, int width, int height);
	// Intended to be used during shutdown.
	static void DestroyRenderer(FoxtrotRenderer* renderer);

public:
	// Changes window resolution.
	void ResizeWindow(FTVector2& windowRes);

	// Clears the screen with clearColor.
	void RenderClear();

	// Presents a rendered image to the user
	// A wrapper to SwapChain->Present(UINT, UINT)
	void SwapChainPresent(UINT syncInterval, UINT flags);

public:
	// D3D11 interfaces (Getters).
	ComPtr<ID3D11Device>&			GetDevice();
	ComPtr<ID3D11DeviceContext>&	GetContext();
	ComPtr<IDXGISwapChain>&			GetSwapChain();
	ComPtr<ID3D11RenderTargetView>& GetRenderTargetView();
	ComPtr<ID3D11DepthStencilView>& GetDSV();
	ComPtr<ID3D11Texture2D>&		GetDepthStencilBuffer();

	ComPtr<ID3D11DepthStencilState>& GetDSS();
	ComPtr<ID3D11DepthStencilState>& GetDSS2D();

	// Shaders without textures (Getters).
	ComPtr<ID3D11VertexShader>& GetSolidVS();
	ComPtr<ID3D11InputLayout>&	GetSolidInputLayout();
	ComPtr<ID3D11PixelShader>&	GetSolidPS();

	// Shaders that render textures (Getters).
	ComPtr<ID3D11VertexShader>& GetTextureVS();
	ComPtr<ID3D11InputLayout>&	GetTextureInputLayout();
	ComPtr<ID3D11PixelShader>&	GetTexturePS();
	ComPtr<ID3D11PixelShader>&	GetBlinnPhongPS();

	// Rendering size related (Getters and Setters).
	UINT GetRenderWidth() const;
	UINT GetRenderHeight() const;
	void SetRenderWidth(const UINT width);
	void SetRenderHeight(const UINT height);

	// FillMode related (Getters and Setters)
	void	 SwitchFillMode() const;
	FillMode GetFillMode() const;
	void	 SetFillMode(const FillMode mode);

private:
	int mRenderWidth;
	int mRenderHeight;

	float	 mClearColor[4];
	UINT	 mNumQualityLevels;
	FillMode mFillMode;

private:
	ComPtr<ID3D11Device>			 mDevice;
	ComPtr<ID3D11DeviceContext>		 mContext;
	ComPtr<ID3D11RenderTargetView>	 mRenderTargetView;
	ComPtr<ID3D11ShaderResourceView> mShaderResourceView;
	ComPtr<IDXGISwapChain>			 mSwapChain;
	ComPtr<ID3D11RasterizerState>	 mSolidRasterizerState;
	ComPtr<ID3D11RasterizerState>	 mWireframeRasterizerState;

	// Depth buffer related
	ComPtr<ID3D11Texture2D>			mDepthStencilBuffer;
	ComPtr<ID3D11DepthStencilView>	mDepthStencilView;
	ComPtr<ID3D11DepthStencilState> mDepthStencilState;
	ComPtr<ID3D11DepthStencilState> mDepthStencilState2D;

	// Texturing
	ComPtr<ID3D11SamplerState> mSamplerState;

	// Viewport
	D3D11_VIEWPORT mScreenViewport;

	// Shaders
	ComPtr<ID3D11VertexShader> mSolidVS;
	ComPtr<ID3D11PixelShader>  mSolidPS;
	ComPtr<ID3D11InputLayout>  mSolidInputLayout;

	ComPtr<ID3D11VertexShader> mTextureVS;
	ComPtr<ID3D11PixelShader>  mTexturePS;
	ComPtr<ID3D11PixelShader>  mBlinnPhongPS;
	ComPtr<ID3D11InputLayout>  mTextureInputLayout;

	ComPtr<ID3D11BlendState> mBlendState;

public:
	FoxtrotRenderer();

private:
	// Intended to be used during initialization.
	bool Initialize(HWND window, int width, int height);

	// ID3D11 Helper functions
	HRESULT CreateRasterizerState();
	HRESULT CreateDepthStencilState(ComPtr<ID3D11DepthStencilState>& dss, bool depthEnabled = true);
	HRESULT CreateBlendState();
	HRESULT CreateTextureSampler();

	void SetViewport(FTVector2 topLeft, FTVector2 resolution);
	void SetViewport(FLOAT topLeftX, FLOAT topLeftY, FLOAT resX, FLOAT resY);

#ifdef FOXTROT_EDITOR
public:
	void RenderToTexture();
	void SetViewport(const ImVec2& topLeft, const ImVec2& resolution);

public:
	RenderTextureClass* GetRenderTexture() { return mRenderTexture; }

private:
	RenderTextureClass* mRenderTexture;
#endif // FOXTROT_EDITOR
};