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

struct Mesh {
    ComPtr<ID3D11Buffer> vertexBuffer;
    ComPtr<ID3D11Buffer> indexBuffer;

    ComPtr<ID3D11Buffer> vertexConstantBuffer;
    ComPtr<ID3D11Buffer> pixelConstantBuffer;

    UINT mIndexCount = 0;
    UINT mVertexCount = 0;
};

// Mesh used for DirectX::GeometricPrimitive
//struct GeometricPrimitiveMesh {
//    std::unique_ptr<DirectX::SpriteBatch> primitive;
//    BasicVertexConstantBuffer basicVertexConstantBufferData;
//};