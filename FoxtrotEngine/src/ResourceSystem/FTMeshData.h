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

	FTDS::DynamicArray<Vertex>	 Vertices;
	FTDS::DynamicArray<uint32_t> Indices;

	FTMeshData operator=(FTMeshData& data)
	{
		size_t i = 0;
		Vertices.IterateArray([&](Vertex& v) {
			v = data.Vertices.At(i);
			++i;
		});

		i = 0;
		Indices.IterateArray([&](uint32_t idx) {
			idx = data.Indices.At(i);
			++i;
		});
		return *this;
	}
};



struct FTDebugMeshData
{
	bool IsEmpty() const;

	std::vector<DebugVertex> Vertices;
	// std::vector<uint32_t>	 Indices;
};