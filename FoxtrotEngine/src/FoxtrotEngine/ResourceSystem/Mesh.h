﻿#pragma once

#include <d3d11.h>
#include <wrl.h> // ComPtr
#include <vector>

//#include "FoxtrotEngine/ResourceSystem/FTTexture.h"
#include "FoxtrotEngine/ResourceSystem/FTTexture.h"
#include "FoxtrotEngine/ResourceSystem/Vertex.h"

using Microsoft::WRL::ComPtr;

struct BasicVertexConstantBuffer {
    DirectX::SimpleMath::Matrix model;
    DirectX::SimpleMath::Matrix view;
    DirectX::SimpleMath::Matrix projection;
};
static_assert((sizeof(BasicVertexConstantBuffer) % 16) == 0,
    "Constant Buffer size must be 16-byte aligned");

struct PixelShaderConstantBuffer {
    float sampleCoordX;
    float sampleCoordY;
    float tileWidth; // 4 * 4 = 16 bytes
    float tileHeight;
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
    FTTexture*           texture = nullptr;

    UINT indexCount = 0;
};

// Mesh used for DirectX::GeometricPrimitive
//struct GeometricPrimitiveMesh {
//    std::unique_ptr<DirectX::SpriteBatch> primitive;
//    BasicVertexConstantBuffer basicVertexConstantBufferData;
//};