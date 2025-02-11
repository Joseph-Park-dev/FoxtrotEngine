// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Constant Data for the Meshes in DirectX 11 context.
/// </summary>

#pragma once

#include <d3d11.h>
#include <wrl.h> // ComPtr
#include <vector>

#include "ResourceSystem/FTTexture.h"
#include "ResourceSystem/Vertex.h"
#include "ResourceSystem/Material.h"
#include "ResourceSystem/Light.h"
#include "Managers/LightManager.h"

using Microsoft::WRL::ComPtr;

struct BasicVertexConstantData {
    DirectX::SimpleMath::Matrix model;
    DirectX::SimpleMath::Matrix invTranspose;
    DirectX::SimpleMath::Matrix view;
    DirectX::SimpleMath::Matrix projection;
};
static_assert((sizeof(BasicVertexConstantData) % 16) == 0,
    "Constant Buffer size must be 16-byte aligned");

struct BasicPixelConstantData
{
    DirectX::SimpleMath::Vector3 EyeWorld;
    bool UseTexture;
    FTMaterial Material;
    Light Lights[GameData::MAX_LIGHTS];
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