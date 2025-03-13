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

// #include "ResourceSystem/FTTexture.h"
#include "ResourceSystem/FTTexture.h"
#include "ResourceSystem/Vertex.h"
#include "ResourceSystem/MeshConstantData.h"

using Microsoft::WRL::ComPtr;

struct Mesh
{
	ComPtr<ID3D11Buffer> VertexBuffer;
	ComPtr<ID3D11Buffer> IndexBuffer;

	std::vector<ComPtr<ID3D11Buffer>> VertexConstantBuffers;
	std::vector<ComPtr<ID3D11Buffer>> PixelConstantBuffers;

	UINT IndexCount	 = 0;
	UINT VertexCount = 0;
};