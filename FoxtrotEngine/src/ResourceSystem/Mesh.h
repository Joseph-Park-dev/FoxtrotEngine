// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Meshes of Foxtrot Engine in DirectX 11 context.
/// </summary>

#pragma once

#include <d3d11.h>
#include <wrl.h> // ComPtr
#include <vector>

//#include "ResourceSystem/FTTexture.h"
#include "ResourceSystem/FTTexture.h"
#include "ResourceSystem/Vertex.h"

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