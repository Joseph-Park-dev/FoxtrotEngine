#pragma once
#include "ResourceSystem/MeshData.h"
#include "ResourceSystem/FTTileMap.h"

class Tile;

class GeometryGenerator {
  public:
    static MeshData MakeSquare(float scale);
    // MakeSquare() for Making tilemap
    static MeshData MakeSquareGrid(const int numSlices, const int numStacks, const float scale, const DirectX::SimpleMath::Vector2 texScale);
    static std::vector<MeshData> MakeTileMapGrid(FTTileMap* tileMap);
    static MeshData MakeTile(Tile* tile);

    static std::vector<MeshData> MakeSpriteAnimation(Tile** tileMap, size_t columnCount, size_t rowCount);
    static MeshData MakeAnimationFrame(Tile* tile);
    static MeshData MakeBox();
    static std::vector<MeshData> ReadFromFile(std::string filePath);

    //static MeshData MakeCylinder(const float bottomRadius,
    //                             const float topRadius, float height,
    //                             int numSlices);
    //static MeshData MakeSphere(const float radius, const int numSlices,
    //                           const int numStacks);
    //static MeshData MakeTetrahedron();
    //static MeshData MakeIcosahedron();
    //static MeshData SubdivideToSphere(const float radius, MeshData meshData);
};