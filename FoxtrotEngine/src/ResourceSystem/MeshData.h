// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// MeshData that defines the ingredients (vertices and indices) to
/// create a Mesh.
/// </summary>

#pragma once
#include <string>

#include "ResourceSystem/Vertex.h"

struct MeshData {
    std::vector<Vertex>   vertices;
    std::vector<uint32_t> indices; // uint32·Î º¯°æ

    bool IsEmpty() const {
        return vertices.empty() || indices.empty();
    }
};