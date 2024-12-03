#pragma once
#include "ResourceSystem/MeshData.h"
#include "ResourceSystem/FTTileMap.h"

class Tile;

class GeometryGenerator {
  public:
    static MeshData MakeSquare();
    static MeshData MakeSquare(float scale);
    // MakeSquare() for Making tilemap
    static MeshData MakeSquareGrid(const int numSlices, const int numStacks, const float scale, const DirectX::SimpleMath::Vector2 texScale);
    static std::vector<MeshData> MakeTileMapGrid(FTTileMap* tileMap);
    static MeshData MakeTile(Tile& tile);

    static void MakeSpriteAnimation(std::vector<MeshData>& animMeshes, Tile* tileMap, size_t columnCount, size_t rowCount);
    static MeshData MakeAnimationFrame(Tile tile);
    static MeshData MakeBox();
};