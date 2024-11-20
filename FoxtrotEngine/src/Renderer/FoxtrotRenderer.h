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
#include "Core\TemplateFunctions.h"

#define VERTEX_SHADER_PATH L"Assets/Shaders/ColorVertexShader.hlsl"
#define PIXEL_SHADER_PATH L"Assets/Shaders/ColorPixelShader.hlsl"

using VertexType = DirectX::VertexPositionColor;
using namespace Microsoft::WRL;
using namespace DX;

class Transform;
class RenderTextureClass;
class FTVector2;

enum class FillMode {
    WireFrame,
    Solid
};

class FoxtrotRenderer
{
public:
	static FoxtrotRenderer* CreateRenderer(HWND window, int width, int height);
	static void             DestroyRenderer(FoxtrotRenderer* renderer);

public:
	void SwapChainPresent(UINT syncInterval, UINT flags);
    void RenderClear();
    void SetViewport();

    void ResizeWindow(UINT width, UINT height);

public:
    ComPtr<ID3D11Device>&            GetDevice()             { return mDevice; }
    ComPtr<ID3D11DeviceContext>&     GetContext()            { return mContext; }
    ComPtr<IDXGISwapChain>&          GetSwapChain()          { return mSwapChain; }
    ComPtr<ID3D11RenderTargetView>&  GetRenderTargetView()   { return mRenderTargetView; }
    ComPtr<ID3D11Texture2D>&         GetDepthStencilBuffer() { return mDepthStencilBuffer; }
    ComPtr<ID3D11InputLayout>&       GetInputLayout()        { return mBasicInputLayout; }

    int      GetRenderWidth ()              { return mRenderWidth; }
    int      GetRenderHeight()              { return mRenderHeight; }
    FillMode GetFillMode    ()              { return mFillMode; }
    void     SwitchFillMode ();

    void  SetRenderWidth        (int width)     { mRenderWidth = width; }
    void  SetRenderHeight       (int height)    { mRenderHeight = height; }
    void  SetFillMode           (FillMode mode) { mFillMode = mode; }

private:
    int         mRenderWidth;
    int         mRenderHeight;
    
    float       mClearColor[4];
    UINT        mNumQualityLevels;
    UINT        mCreateDeviceFlags;

    FillMode    mFillMode;

private:
    ComPtr<ID3D11BlendState>        mBlendState;

	ComPtr<ID3D11Device>			mDevice;
	ComPtr<ID3D11DeviceContext>		mContext;
	ComPtr<ID3D11RenderTargetView>	mRenderTargetView;
	ComPtr<IDXGISwapChain>			mSwapChain;
    ComPtr<ID3D11RasterizerState>	mSolidRasterizerState;
    ComPtr<ID3D11RasterizerState>	mWireframeRasterizerState;

	// Depth buffer 관련
	ComPtr<ID3D11Texture2D>			mDepthStencilBuffer;
	ComPtr<ID3D11DepthStencilView>	mDepthStencilView;
	ComPtr<ID3D11DepthStencilState> mDepthStencilState;

	// Texturing
	ComPtr<ID3D11SamplerState> mSamplerState;

	D3D11_VIEWPORT mScreenViewport;

    ComPtr<ID3D11VertexShader> mBasicVertexShader;
    ComPtr<ID3D11PixelShader>  mBasicPixelShader;
    ComPtr<ID3D11InputLayout>  mBasicInputLayout;

public:
    FoxtrotRenderer();

private:
	bool Initialize(HWND window, int width, int height);

private:
    HRESULT CreateDeviceAndContext(HWND window);
    HRESULT CreateRenderTargetView();
    HRESULT CreateRasterizerState();
    HRESULT CreateDepthBuffer();
    HRESULT CreateDepthStencilState();
    HRESULT CreateBlendState();
    HRESULT CreateTextureSampler();

public:
    bool UpdateDepthBuffer(int width, int height);
    void CreateIndexBuffer(const std::vector<uint32_t>& indices, ComPtr<ID3D11Buffer>& m_indexBuffer);
    template <typename T_VERTEX>
    void CreateVertexBuffer(const std::vector<T_VERTEX>& vertices,
        ComPtr<ID3D11Buffer>& vertexBuffer) {

        // D3D11_USAGE enumeration (d3d11.h)
        // https://learn.microsoft.com/en-us/windows/win32/api/d3d11/ne-d3d11-d3d11_usage

        D3D11_BUFFER_DESC bufferDesc;
        ZeroMemory(&bufferDesc, sizeof(bufferDesc));
        bufferDesc.Usage = D3D11_USAGE_DYNAMIC; // 초기화 후 변경X
        bufferDesc.ByteWidth = UINT(sizeof(T_VERTEX) * vertices.size());
        bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // 0 if no CPU access is necessary.
        bufferDesc.StructureByteStride = sizeof(T_VERTEX);

        D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 }; // MS 예제에서 초기화하는 방식
        vertexBufferData.pSysMem = vertices.data();
        vertexBufferData.SysMemPitch = 0;
        vertexBufferData.SysMemSlicePitch = 0;

        const HRESULT hr = mDevice->CreateBuffer(
            &bufferDesc, &vertexBufferData, vertexBuffer.GetAddressOf());
        if (FAILED(hr)) {
            std::cout << "CreateBuffer() failed. " << std::hex << hr
                << std::endl;
        };
    }
    template <typename T_CONSTANT>
    void CreateConstantBuffer(const T_CONSTANT& constantBufferData,
        ComPtr<ID3D11Buffer>& constantBuffer) {
        // 주의:
        // For a constant buffer (BindFlags of D3D11_BUFFER_DESC set to
        // D3D11_BIND_CONSTANT_BUFFER), you must set the ByteWidth value of
        // D3D11_BUFFER_DESC in multiples of 16, and less than or equal to
        // D3D11_REQ_CONSTANT_BUFFER_ELEMENT_COUNT.

        D3D11_BUFFER_DESC cbDesc;
        cbDesc.ByteWidth = sizeof(constantBufferData);
        cbDesc.Usage = D3D11_USAGE_DYNAMIC;
        cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        cbDesc.MiscFlags = 0;
        cbDesc.StructureByteStride = 0;

        // Fill in the subresource data.
        D3D11_SUBRESOURCE_DATA initData;
        initData.pSysMem = &constantBufferData;
        initData.SysMemPitch = 0;
        initData.SysMemSlicePitch = 0;

        auto hr = mDevice->CreateBuffer(&cbDesc, &initData,
            constantBuffer.GetAddressOf());
        if (FAILED(hr)) {
            std::cout << "CreateConstantBuffer() CreateBuffer failed()."
                << std::endl;
        }
    }
    template <typename T_DATA>
    void UpdateBuffer(const T_DATA& bufferData, ComPtr<ID3D11Buffer>& buffer) {

        if (!buffer) {
            std::cout << "UpdateBuffer() buffer was not initialized."
                << std::endl;
        }

        D3D11_MAPPED_SUBRESOURCE ms;
        DX::ThrowIfFailed(mContext->Map(buffer.Get(), NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms));
        memcpy(ms.pData, &bufferData, sizeof(bufferData));
        mContext->Unmap(buffer.Get(), NULL);
    }

#ifdef FOXTROT_EDITOR
public:
	void RenderToTexture();

public:
    RenderTextureClass* GetRenderTexture() { return mRenderTexture; }
    
private:
	RenderTextureClass* mRenderTexture;
#endif // FOXTROT_EDITOR
};