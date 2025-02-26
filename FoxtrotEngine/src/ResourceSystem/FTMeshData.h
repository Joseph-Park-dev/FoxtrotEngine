// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
//
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Defines the vertices and indices to create a Mesh.
/// </summary>

#pragma once
#include "ResourceSystem/FTResource.h"

#include <vector>

struct Vertex;

class FTMeshData :
	public FTResource
{
public:
	std::vector<Vertex>&   GetVertices();
	std::vector<uint32_t>& GetIndices();

	const size_t VerticesCount() const;
	const size_t IndicesCount() const;
	bool		 IsEmpty() const;

public:
	FTMeshData();

private:
	std::vector<Vertex>	  mVertices;
	std::vector<uint32_t> mIndices;
};
