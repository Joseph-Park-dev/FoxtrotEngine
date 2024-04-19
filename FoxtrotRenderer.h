#pragma once
#include <windows.h>
#include <directxtk/SimpleMath.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <wrl.h> // ComPtr
#include <iostream>

#include "FTTexture.h"

using namespace Microsoft::WRL;

struct Vertex {
    DirectX::SimpleMath::Vector3 position;
    DirectX::SimpleMath::Vector3 color;
    DirectX::SimpleMath::Vector2 texcoord;
};

struct ModelViewProjectionConstantBuffer {
    DirectX::SimpleMath::Matrix model;
    DirectX::SimpleMath::Matrix view;
    DirectX::SimpleMath::Matrix projection;
};
static_assert((sizeof(ModelViewProjectionConstantBuffer) % 16) == 0,
    "Constant Buffer size must be 16-byte aligned");

struct PixelShaderConstantBuffer {
    float xSplit; // 4 bytes
    float padding[3]; // 4 * 3 = 12 bytes
};
static_assert((sizeof(PixelShaderConstantBuffer) % 16) == 0,
    "Constant Buffer size must be 16-byte aligned");

class FoxtrotRenderer
{
public:
	static FoxtrotRenderer* CreateRenderer(HWND window, int width, int height);
	static void DestroyRenderer(FoxtrotRenderer* renderer);

public:
	void SwapChainPresent(UINT syncInterval, UINT flags);
    void Update();
    void Render();
	void RenderClear(const float clearColor[4]);

public:
	ID3D11Device*        GetDevice()  { return mDevice.Get(); }
	ID3D11DeviceContext* GetContext() { return mContext.Get(); }

private:
    int mRenderWidth;
    int mRenderHeight;

private:
    FTTexture mWallTexture;

    ComPtr<ID3D11BlendState>        mBlendState;

	ComPtr<ID3D11Device>			mDevice;
	ComPtr<ID3D11DeviceContext>		mContext;
	ComPtr<ID3D11RenderTargetView>	mRenderTargetView;
	ComPtr<IDXGISwapChain>			mSwapChain;
	ComPtr<ID3D11RasterizerState>	mRasterizerState;

	// Depth buffer 관련
	ComPtr<ID3D11Texture2D>			mDepthStencilBuffer;
	ComPtr<ID3D11DepthStencilView>	mDepthStencilView;
	ComPtr<ID3D11DepthStencilState> mDepthStencilState;

	// Texturing
	ComPtr<ID3D11SamplerState> mSamplerState;

	D3D11_VIEWPORT mScreenViewport;

    ComPtr<ID3D11VertexShader> mColorVertexShader;
    ComPtr<ID3D11PixelShader> mColorPixelShader;
    ComPtr<ID3D11InputLayout> mColorInputLayout;

    ComPtr<ID3D11Buffer> mVertexBuffer;
    ComPtr<ID3D11Buffer> mIndexBuffer;
    ComPtr<ID3D11Buffer> mConstantBuffer;
    ComPtr<ID3D11Buffer> mPixelShaderConstantBuffer;
    UINT mIndexCount;

    ModelViewProjectionConstantBuffer mConstantBufferData;
    PixelShaderConstantBuffer mPixelShaderConstantBufferData;

    bool m_usePerspectiveProjection = true;
    DirectX::SimpleMath::Vector3 mModelTranslation = DirectX::SimpleMath::Vector3(0.0f);
    DirectX::SimpleMath::Vector3 mModelRotation = DirectX::SimpleMath::Vector3(0.0f);
    DirectX::SimpleMath::Vector3 mModelScaling = DirectX::SimpleMath::Vector3(0.5f);
    DirectX::SimpleMath::Vector3 mViewEyePos = { 0.0f, 0.0f, -2.0f };
    DirectX::SimpleMath::Vector3 mViewEyeDir = { 0.0f, 0.0f, 1.0f };
    DirectX::SimpleMath::Vector3 mViewUp = { 0.0f, 1.0f, 0.0f };
    float mProjFovAngleY = 70.0f;
    float mNearZ = 0.01f;
    float mFarZ = 100.0f;
    float mAspect = GetAspectRatio();

    float GetAspectRatio() const {
        return float(1920) / float(1080);
    }

public:
    FoxtrotRenderer();

private:
	bool Initialize(HWND window, int width, int height);
    bool CreateDeviceAndContext(HWND window, int width, int height);
    bool CreateSwapChain(const D3D_DRIVER_TYPE driverType);
    bool CreateRenderTarget();
    void CreateIndexBuffer(const std::vector<uint16_t>& indices, ComPtr<ID3D11Buffer>& m_indexBuffer);

    template <typename T_VERTEX>
    void CreateVertexBuffer(const std::vector<T_VERTEX>& vertices,
        ComPtr<ID3D11Buffer>& vertexBuffer) {

        // D3D11_USAGE enumeration (d3d11.h)
        // https://learn.microsoft.com/en-us/windows/win32/api/d3d11/ne-d3d11-d3d11_usage

        D3D11_BUFFER_DESC bufferDesc;
        ZeroMemory(&bufferDesc, sizeof(bufferDesc));
        bufferDesc.Usage = D3D11_USAGE_IMMUTABLE; // 초기화 후 변경X
        bufferDesc.ByteWidth = UINT(sizeof(T_VERTEX) * vertices.size());
        bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bufferDesc.CPUAccessFlags = 0; // 0 if no CPU access is necessary.
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
        mContext->Map(buffer.Get(), NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
        memcpy(ms.pData, &bufferData, sizeof(bufferData));
        mContext->Unmap(buffer.Get(), NULL);
    }

    void CreateVertexShaderAndInputLayout(
        const std::wstring& filename, 
        const std::vector<D3D11_INPUT_ELEMENT_DESC>& inputElements, 
        ComPtr<ID3D11VertexShader>& vertexShader, 
        ComPtr<ID3D11InputLayout>& inputLayout
    );
    void CreatePixelShader(const std::wstring& filename, ComPtr<ID3D11PixelShader>& pixelShader);
};