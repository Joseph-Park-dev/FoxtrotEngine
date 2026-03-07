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
#define TEXTURE_PS_2D_PATH L"Assets/Shaders/2D/Texture2DPS.hlsl"
#define RIM_TEXTURE_PS_PATH L"Assets/Shaders/RimTexturePS.hlsl"

#define NORMAL_VS_PATH L"Assets/Shaders/NormalVS.hlsl"
#define NORMAL_PS_PATH L"Assets/Shaders/NormalPS.hlsl"

#define CUBEMAP_VS_PATH L"Assets/Shaders/CubeMapVS.hlsl"
#define CUBEMAP_PS_PATH L"Assets/Shaders/CubeMapPS.hlsl"

using VertexType = DirectX::VertexPositionColor;
using namespace Microsoft::WRL;
using namespace DX;

class Transform;
class ViewportRenderer;
class FTVector2;
class FTWindow;

enum class FillMode
{
	WireFrame,
	Solid
};

class FoxtrotRenderer
{
//public:
//	// Intended to be used during intialization.
//	static FoxtrotRenderer* CreateRenderer(FTWindow* window, int width, int height);
//	// Intended to be used during shutdown.
//	static void DestroyRenderer(FoxtrotRenderer* renderer);

public:
	// D3D11 interfaces (Getters).
	ComPtr<ID3D11Device>&		 GetDevice();
	ComPtr<ID3D11DeviceContext>& GetContext();
	ComPtr<ID3D11BlendState>&	 GetBlendState();

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
	ComPtr<ID3D11PixelShader>&	GetTexture2DPS();
	ComPtr<ID3D11PixelShader>&	GetRimTexturePS();

	// Shaders that render normal lines (Getters).
	ComPtr<ID3D11VertexShader>& GetNormalVS();
	ComPtr<ID3D11PixelShader>&	GetNormalPS();

	ComPtr<ID3D11VertexShader>& GetCubeMapVS();
	ComPtr<ID3D11PixelShader>&	GetCubeMapPS();

	const float* GetClearColor() const;
	UINT		 GetNumQualityLevels();
	uint8_t*	 GetCursorPosColor();

	// FillMode related (Getters and Setters)
	void	 SwitchFillMode() const;
	FillMode GetFillMode() const;
	void	 SetFillMode(const FillMode mode);

private:
	float	 mClearColor[4];
	UINT	 mNumQualityLevels;
	FillMode mFillMode;

	// Pixel color (RGBA) at the cursor position.
	uint8_t mCursorPosColor[4] = {
		0,
	};

private:
	ComPtr<ID3D11Device>		  mDevice;
	ComPtr<ID3D11DeviceContext>	  mContext;
	ComPtr<ID3D11RasterizerState> mSolidRasterizerState;
	ComPtr<ID3D11RasterizerState> mWireframeRasterizerState;

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
	ComPtr<ID3D11PixelShader>  mTexture2DPS;
	ComPtr<ID3D11PixelShader>  mRimTexturePS;
	ComPtr<ID3D11InputLayout>  mTextureInputLayout;

	ComPtr<ID3D11VertexShader> mNormalVS;
	ComPtr<ID3D11PixelShader>  mNormalPS;

	ComPtr<ID3D11VertexShader> mCubeMapVS;
	ComPtr<ID3D11PixelShader>  mCubeMapPS;

	ComPtr<ID3D11BlendState> mBlendState;

public:
	FoxtrotRenderer();
	~FoxtrotRenderer();

private:
	// Intended to be used during initialization.
	bool Initialize(FTWindow* window, int renderWidth, int renderHeight);

	// ID3D11 Helper functions
	HRESULT CreateRasterizerState();
	HRESULT CreateDepthStencilState(ComPtr<ID3D11DepthStencilState>& dss, bool depthEnabled = true);
	HRESULT CreateBlendState();
	HRESULT CreateTextureSampler();

#ifdef FOXTROT_EDITOR
public:
	bool InitializeViewport(FTWindow* window, UINT posX, UINT posY, UINT width, UINT height);
	void RenderOnViewport();
	void SetViewport(const ImVec2& topLeft, const ImVec2& resolution);

public:
	ViewportRenderer* GetViewportRenderer();

private:
	ViewportRenderer* mViewportRenderer;
#endif // FOXTROT_EDITOR
};