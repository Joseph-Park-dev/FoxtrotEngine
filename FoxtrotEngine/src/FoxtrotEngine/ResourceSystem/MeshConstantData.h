#pragma once

#include <d3d11.h>
#include <wrl.h> // ComPtr
#include <vector>

#include "FoxtrotEngine/ResourceSystem/FTTexture.h"
#include "FoxtrotEngine/ResourceSystem/Vertex.h"

using Microsoft::WRL::ComPtr;

struct BasicVertexConstantData {
    DirectX::SimpleMath::Matrix model;
    DirectX::SimpleMath::Matrix invTranspose;
    DirectX::SimpleMath::Matrix view;
    DirectX::SimpleMath::Matrix projection;
};
static_assert((sizeof(BasicVertexConstantData) % 16) == 0,
    "Constant Buffer size must be 16-byte aligned");

struct BasicPixelConstantData {
    float dummy1;
    float dummy2;
    float dummy3;
    float dummy4;
};
static_assert((sizeof(BasicPixelConstantData) % 16) == 0,
    "Constant Buffer size must be 16-byte aligned");

struct TileMapConstantData {
    float texCoordX;
    float texCoordY;
    float texSizeX;
    float texSizeY;
};
static_assert((sizeof(TileMapConstantData) % 16) == 0,
    "Constant Buffer size must be 16-byte aligned");