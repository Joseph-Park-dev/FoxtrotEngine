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
#include "ResourceSystem/FTShaders/StandardPS.h"
#include "ResourceSystem/Light.h"
#include "Managers/LightManager.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX::SimpleMath;

struct BasicVCData
{
	DirectX::SimpleMath::Matrix model		 = DirectX::SimpleMath::Matrix();
	DirectX::SimpleMath::Matrix invTranspose = DirectX::SimpleMath::Matrix();
	DirectX::SimpleMath::Matrix view		 = DirectX::SimpleMath::Matrix();
	DirectX::SimpleMath::Matrix projection	 = DirectX::SimpleMath::Matrix();
};
static_assert((sizeof(BasicVCData) % 16) == 0, "Constant Buffer size must be 16-byte aligned");

// Debug shape only takes a point in VS, 
// and utilizes that in GS to form a geometry
struct DebugVCData
{
	DirectX::SimpleMath::Matrix model = DirectX::SimpleMath::Matrix();
};

static_assert((sizeof(DebugVCData) % 16) == 0, "Constant Buffer size must be 16-byte aligned");

struct DebugGCData
{
	DirectX::SimpleMath::Matrix view = DirectX::SimpleMath::Matrix();
	DirectX::SimpleMath::Matrix projection = DirectX::SimpleMath::Matrix();

	Vector2 size;
	Vector2 dummy;
};

static_assert((sizeof(DebugGCData) % 16) == 0, "Constant Buffer size must be 16-byte aligned");

struct SpineMeshVCData
{
	std::vector<DirectX::SimpleMath::Matrix> transformation;
};

static_assert((sizeof(SpineMeshVCData) % 16) == 0, "Constant Buffer size must be 16-byte aligned");

struct NormalVCData
{
	float scale = 0.1f;
	float dummy[3];
};
static_assert((sizeof(NormalVCData) % 16) == 0, "Constant Buffer size must be 16-byte aligned");

struct TileMapConstantData
{
	float texCoordX;
	float texCoordY;
	float texSizeX;
	float texSizeY;
};
static_assert((sizeof(TileMapConstantData) % 16) == 0, "Constant Buffer size must be 16-byte aligned");

// Data use for Mouse Picking
struct IndexPCData
{
	DirectX::SimpleMath::Vector4 IndexColor;
	uint32_t					 IsActive;
	uint32_t					 dummy[3];
};

struct DebugPCData
{
	uint32_t					 IsActive;
	uint32_t					 dummy[3];
};