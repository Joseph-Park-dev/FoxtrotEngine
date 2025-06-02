// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Defines the vertices and indices to create a Mesh.
/// This will be included in FTMeshDataPack
/// </summary>

#pragma once
#include "ResourceSystem/FTResource.h"

#include <vector>

#include "ResourceSystem/Vertex.h"

struct FTMeshData
{
	bool IsEmpty() const;

	std::vector<Vertex>	  Vertices;
	std::vector<uint32_t> Indices;
};

struct FTDebugMeshData
{
	bool IsEmpty() const;

	std::vector<DebugVertex> Vertices;
	std::vector<uint32_t>	 Indices;
};