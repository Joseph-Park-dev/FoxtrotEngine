#pragma once

#include <d3d11.h>
#include <wrl.h> // ComPtr
#include <vector>

#include "FTTexture.h"

using Microsoft::WRL::ComPtr;
class Vertex;

struct BasicVertexConstantBuffer {
    DirectX::SimpleMath::Matrix model;
    DirectX::SimpleMath::Matrix view;
    DirectX::SimpleMath::Matrix projection;
};
static_assert((sizeof(BasicVertexConstantBuffer) % 16) == 0,
    "Constant Buffer size must be 16-byte aligned");

struct PixelShaderConstantBuffer {
    float xSplit; // 4 bytes
    float padding[3]; // 4 * 3 = 12 bytes
};
static_assert((sizeof(PixelShaderConstantBuffer) % 16) == 0,
    "Constant Buffer size must be 16-byte aligned");

// 같은 메쉬를 여러번 그릴 때 버퍼들을 재사용
struct Mesh {
    BasicVertexConstantBuffer basicVertexConstantBufferData;
    PixelShaderConstantBuffer pixelShaderConstantBufferData;

    ComPtr<ID3D11Buffer> vertexBuffer;
    ComPtr<ID3D11Buffer> indexBuffer;
    ComPtr<ID3D11Buffer> vertexConstantBuffer;
    ComPtr<ID3D11Buffer> pixelConstantBuffer;
    FTTexture*           texture;

    UINT m_indexCount = 0;
};