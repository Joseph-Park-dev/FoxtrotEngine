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
#include "ResourceSystem/MeshConstantData.h"

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

struct Mesh 
{
    Mesh()
    {
        VertexConstantData.model = DirectX::SimpleMath::Matrix();
        VertexConstantData.view = DirectX::SimpleMath::Matrix();
        VertexConstantData.projection = DirectX::SimpleMath::Matrix();
    }

    ComPtr<ID3D11Buffer> VertexBuffer;
    ComPtr<ID3D11Buffer> IndexBuffer;

    ComPtr<ID3D11Buffer> VertexConstantBuffer;
    ComPtr<ID3D11Buffer> PixelConstantBuffer;

    BasicVertexConstantData VertexConstantData;
    BasicPixelConstantData	PixelConstantData;

    UINT IndexCount = 0;
    UINT VertexCount = 0;
};

// Mesh used for DirectX::GeometricPrimitive
//struct GeometricPrimitiveMesh {
//    std::unique_ptr<DirectX::SpriteBatch> primitive;
//    BasicVertexConstantBuffer basicVertexConstantBufferData;
//};