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
#include "ResourceSystem/Mesh/Vertex.h"
#include "FTDS/Dynamic/DynamicArray.h"

namespace D3D11
{
	struct FTMeshData
	{
		bool IsEmpty() const;

		Common::FTDS::DynamicArray<Vertex>   Vertices;
		Common::FTDS::DynamicArray<uint32_t> Indices;

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

		Common::FTDS::DynamicArray<DebugVertex> Vertices;
		// std::vector<uint32_t>	 Indices;
	};
} // namespace D3D11