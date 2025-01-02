// ----------------------------------------------------------------
// Foxtrot Engine 2D
// Copyright (C) 2025 JungBae Park. All rights reserved.
// 
// Released under the GNU General Public License v3.0
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
/// <summary>
/// Generates the MeshData that is used to make Primitive Meshes.
/// </summary>

#pragma once
#include "ResourceSystem/MeshData.h"
#include "ResourceSystem/FTTileMap.h"

class Tile;

class GeometryGenerator {
  public:
    static MeshData MakeSquare();
    static MeshData MakeSquare(float scale);
    static MeshData MakeSquare(FTVector3 color);
    static MeshData MakeSquare(float scale, FTVector3 color);
    // MakeSquare() for Making tilemap
    static MeshData MakeSquareGrid(const int numSlices, const int numStacks, const float scale, const DirectX::SimpleMath::Vector2 texScale);
    static std::vector<MeshData> MakeTileMapGrid(FTTileMap* tileMap);
    static MeshData MakeTile(Tile& tile);

    static void MakeSpriteAnimation(std::vector<MeshData>& animMeshes, Tile* tileMap, size_t columnCount, size_t rowCount);
    static MeshData MakeAnimationFrame(Tile tile);
    static MeshData MakeBox();
};