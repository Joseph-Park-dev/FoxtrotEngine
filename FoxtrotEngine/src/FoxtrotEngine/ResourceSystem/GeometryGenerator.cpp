#include "FoxtrotEngine/ResourceSystem/GeometryGenerator.h"

#include <directxtk/SimpleMath.h>
#include <vector>

#include "FoxtrotEngine/Core/TemplateFunctions.h"
#include "FoxtrotEngine/Math/FTMath.h"
#include "FoxtrotEngine/ResourceSystem/Vertex.h"
#include "FoxtrotEngine/ResourceSystem/MeshData.h"
#include "FoxtrotEngine/ResourceSystem/FTTexture.h"
#include "FoxtrotEngine/ResourceSystem/ModelLoader.h"
#include "FoxtrotEngine/ResourceSystem/Tile.h"

//#include "ModelLoader.h"
//using namespace DirectX;
//using namespace DirectX::SimpleMath;
//using D3DVec2 = DirectX::SimpleMath::Vector2;
//using D3DVec3 = DirectX::SimpleMath::Vector3;

using Vector3 = DirectX::SimpleMath::Vector3;
using Vector2 = DirectX::SimpleMath::Vector2;

MeshData GeometryGenerator::MakeSquare(float scale) {
    std::vector<Vector3> positions;
    std::vector<Vector3> colors;
    std::vector<Vector3> normals;
    std::vector<Vector2> texcoords; // 텍스춰 좌표

    // 앞면
    positions.push_back(Vector3(-1.0f, 1.0f, 0.0f) * scale);
    positions.push_back(Vector3(1.0f, 1.0f, 0.0f) * scale);
    positions.push_back(Vector3(1.0f, -1.0f, 0.0f) * scale);
    positions.push_back(Vector3(-1.0f, -1.0f, 0.0f) * scale);
    colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
    colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
    colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
    colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));

    // Texture Coordinates (Direct3D 9)
    // https://learn.microsoft.com/en-us/windows/win32/direct3d9/texture-coordinates
    texcoords.push_back(Vector2(0.0f, 0.0f));
    texcoords.push_back(Vector2(1.0f, 0.0f));
    texcoords.push_back(Vector2(1.0f, 1.0f));
    texcoords.push_back(Vector2(0.0f, 1.0f));

    MeshData meshData;
    for (size_t i = 0; i < positions.size(); i++) {
        Vertex v;
        v.position = positions[i];
        v.color = colors[i];
        v.normal = normals[i];
        v.texcoord = texcoords[i];
        meshData.vertices.push_back(v);
    }
    meshData.indices = {
        0, 1, 2, 0, 2, 3, // 앞면
    };

    return meshData;
}

MeshData GeometryGenerator::MakeSquareGrid(
    const int numSlices,
    const int numStacks,
    const float scale,
    const Vector2 texScale) {
    MeshData meshData;

    float dx = 2.0f / numSlices;
    float dy = 2.0f / numStacks;

    float y = 1.0f;
    for (int j = 0; j < numStacks + 1; j++) {
        float x = -1.0f;
        for (int i = 0; i < numSlices + 1; i++) {
            Vertex v;
            v.position = Vector3(x, y, 0.0f) * scale;
            v.texcoord = Vector2(1.0f) - (Vector2(-x + 1.0f, y + 1.0f) * 0.5f * texScale);

            printf("%f    %f\n", v.texcoord.x, v.texcoord.y);

            meshData.vertices.push_back(v);

            x += dx;
        }
        y -= dy;
    }

    for (int j = 0; j < numStacks; j++) {
        for (int i = 0; i < numSlices; i++) {
            meshData.indices.push_back((numSlices + 1) * j + i);
            meshData.indices.push_back((numSlices + 1) * j + i + 1);
            meshData.indices.push_back((numSlices + 1) * (j + 1) + i);
            meshData.indices.push_back((numSlices + 1) * (j + 1) + i);
            meshData.indices.push_back((numSlices + 1) * j + i + 1);
            meshData.indices.push_back((numSlices + 1) * (j + 1) + i + 1);
        }
    }
    return meshData;
}

std::vector<MeshData> GeometryGenerator::MakeTileMapGrid(Tile** tileMap, size_t columnCount, size_t rowCount)
{
    std::vector<MeshData> tileMapMeshes;
    for (size_t r = 0; r < rowCount; ++r) {
        for (size_t c = 0; c < columnCount; ++c) {
            MeshData tileMesh = MakeTile(&tileMap[r][c]);
            tileMapMeshes.push_back(tileMesh);
        }
    }
    return tileMapMeshes;
}

MeshData GeometryGenerator::MakeTile(Tile* tile)
{
    std::vector<Vector3> positions;
    std::vector<Vector3> colors;
    std::vector<Vector3> normals;
    std::vector<Vector2> texcoords; // 텍스춰 좌표

    FTRect* rectOnScreen = tile->GetRectOnScreen();

    const FTVector2& tileMin = rectOnScreen->GetMin();
    const FTVector2& tileMax = rectOnScreen->GetMax();
    const float tileWidth = rectOnScreen->GetSize().x;
    const float tileHeight = rectOnScreen->GetSize().y;

    // 앞면
    positions.push_back(Vector3(tileMin.x,              -tileMin.y,              0.0f));
    positions.push_back(Vector3(tileMin.x + tileWidth,  -tileMin.y,              0.0f));
    positions.push_back(Vector3(tileMin.x + tileWidth,  -tileMin.y - tileHeight, 0.0f));
    positions.push_back(Vector3(tileMin.x,              -tileMin.y - tileHeight, 0.0f));
    colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
    colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
    colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
    colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));

    // Texture Coordinates (Direct3D 9)
    // https://learn.microsoft.com/en-us/windows/win32/direct3d9/texture-coordinates
    FTRect* rectOnMap = tile->GetRectOnMap();
    const FTVector2& mapMin = rectOnMap->GetMin();
    const float widthInMap = rectOnMap->GetSize().x;
    const float heightInMap = rectOnMap->GetSize().y;

    texcoords.push_back(Vector2(mapMin.x,               mapMin.y));
    texcoords.push_back(Vector2(mapMin.x + widthInMap,  mapMin.y));
    texcoords.push_back(Vector2(mapMin.x + widthInMap,  mapMin.y + heightInMap));
    texcoords.push_back(Vector2(mapMin.x,               mapMin.y + heightInMap));

    MeshData meshData;
    for (size_t i = 0; i < positions.size(); i++) {
        Vertex v;
        v.position = positions[i];
        v.color = colors[i];
        v.normal = normals[i];
        v.texcoord = texcoords[i];
        meshData.vertices.push_back(v);
    }
    meshData.indices = {
        0, 1, 2, 0, 2, 3, // 앞면
    };

    return meshData;
}

std::vector<MeshData> GeometryGenerator::MakeSpriteAnimation(Tile** tileMap, size_t columnCount, size_t rowCount)
{
    std::vector<MeshData> animationMeshes;
    for (size_t r = 0; r < rowCount; ++r) {
        for (size_t c = 0; c < columnCount; ++c) {
            MeshData animFrame = MakeAnimationFrame(&tileMap[r][c]);
            animationMeshes.push_back(animFrame);
        }
    }
    return animationMeshes;
}

MeshData GeometryGenerator::MakeAnimationFrame(Tile* tile)
{
    std::vector<Vector3> positions;
    std::vector<Vector3> colors;
    std::vector<Vector3> normals;
    std::vector<Vector2> texcoords; // 텍스춰 좌표

    // 앞면
    positions.push_back(Vector3(-1.0f, 1.0f, 0.0f));
    positions.push_back(Vector3(1.0f, 1.0f, 0.0f));
    positions.push_back(Vector3(1.0f, -1.0f, 0.0f));
    positions.push_back(Vector3(-1.0f, -1.0f, 0.0f));
    colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
    colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
    colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
    colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));

    // Texture Coordinates (Direct3D 9)
    // https://learn.microsoft.com/en-us/windows/win32/direct3d9/texture-coordinates
    FTRect* rectOnMap = tile->GetRectOnMap();
    const FTVector2& mapMin = rectOnMap->GetMin();
    const float widthInMap = rectOnMap->GetSize().x;
    const float heightInMap = rectOnMap->GetSize().y;

    texcoords.push_back(Vector2(mapMin.x, mapMin.y));
    texcoords.push_back(Vector2(mapMin.x + widthInMap, mapMin.y));
    texcoords.push_back(Vector2(mapMin.x + widthInMap, mapMin.y + heightInMap));
    texcoords.push_back(Vector2(mapMin.x, mapMin.y + heightInMap));

    MeshData meshData;
    for (size_t i = 0; i < positions.size(); i++) {
        Vertex v;
        v.position = positions[i];
        v.color = colors[i];
        v.normal = normals[i];
        v.texcoord = texcoords[i];
        meshData.vertices.push_back(v);
    }
    meshData.indices = {
        0, 1, 2, 0, 2, 3, // 앞면
    };

    return meshData;
}

MeshData GeometryGenerator::MakeBox() {
    std::vector<Vector3> positions;
    std::vector<Vector3> colors;
    std::vector<Vector3> normals;
    std::vector<Vector2> texcoords; // 텍스춰 좌표

    const float scale = 1.0f;

    // 윗면
    positions.push_back(Vector3(-1.0f, 1.0f, -1.0f) * scale);
    positions.push_back(Vector3(-1.0f, 1.0f, 1.0f) * scale);
    positions.push_back(Vector3(1.0f, 1.0f, 1.0f) * scale);
    positions.push_back(Vector3(1.0f, 1.0f, -1.0f) * scale);
    colors.push_back(Vector3(1.0f, 0.0f, 0.0f));
    colors.push_back(Vector3(1.0f, 0.0f, 0.0f));
    colors.push_back(Vector3(1.0f, 0.0f, 0.0f));
    colors.push_back(Vector3(1.0f, 0.0f, 0.0f));
    normals.push_back(Vector3(0.0f, 1.0f, 0.0f));
    normals.push_back(Vector3(0.0f, 1.0f, 0.0f));
    normals.push_back(Vector3(0.0f, 1.0f, 0.0f));
    normals.push_back(Vector3(0.0f, 1.0f, 0.0f));
    texcoords.push_back(Vector2(0.0f, 0.0f));
    texcoords.push_back(Vector2(1.0f, 0.0f));
    texcoords.push_back(Vector2(1.0f, 1.0f));
    texcoords.push_back(Vector2(0.0f, 1.0f));

    // 아랫면
    positions.push_back(Vector3(-1.0f, -1.0f, -1.0f) * scale);
    positions.push_back(Vector3(1.0f, -1.0f, -1.0f) * scale);
    positions.push_back(Vector3(1.0f, -1.0f, 1.0f) * scale);
    positions.push_back(Vector3(-1.0f, -1.0f, 1.0f) * scale);
    colors.push_back(Vector3(0.0f, 1.0f, 0.0f));
    colors.push_back(Vector3(0.0f, 1.0f, 0.0f));
    colors.push_back(Vector3(0.0f, 1.0f, 0.0f));
    colors.push_back(Vector3(0.0f, 1.0f, 0.0f));
    normals.push_back(Vector3(0.0f, -1.0f, 0.0f));
    normals.push_back(Vector3(0.0f, -1.0f, 0.0f));
    normals.push_back(Vector3(0.0f, -1.0f, 0.0f));
    normals.push_back(Vector3(0.0f, -1.0f, 0.0f));
    texcoords.push_back(Vector2(0.0f, 0.0f));
    texcoords.push_back(Vector2(1.0f, 0.0f));
    texcoords.push_back(Vector2(1.0f, 1.0f));
    texcoords.push_back(Vector2(0.0f, 1.0f));

    // 앞면
    positions.push_back(Vector3(-1.0f, -1.0f, -1.0f) * scale);
    positions.push_back(Vector3(-1.0f, 1.0f, -1.0f) * scale);
    positions.push_back(Vector3(1.0f, 1.0f, -1.0f) * scale);
    positions.push_back(Vector3(1.0f, -1.0f, -1.0f) * scale);
    colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
    colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
    colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
    colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
    texcoords.push_back(Vector2(0.0f, 0.0f));
    texcoords.push_back(Vector2(1.0f, 0.0f));
    texcoords.push_back(Vector2(1.0f, 1.0f));
    texcoords.push_back(Vector2(0.0f, 1.0f));

    // 뒷면
    positions.push_back(Vector3(-1.0f, -1.0f, 1.0f) * scale);
    positions.push_back(Vector3(1.0f, -1.0f, 1.0f) * scale);
    positions.push_back(Vector3(1.0f, 1.0f, 1.0f) * scale);
    positions.push_back(Vector3(-1.0f, 1.0f, 1.0f) * scale);
    colors.push_back(Vector3(0.0f, 1.0f, 1.0f));
    colors.push_back(Vector3(0.0f, 1.0f, 1.0f));
    colors.push_back(Vector3(0.0f, 1.0f, 1.0f));
    colors.push_back(Vector3(0.0f, 1.0f, 1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, 1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, 1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, 1.0f));
    normals.push_back(Vector3(0.0f, 0.0f, 1.0f));
    texcoords.push_back(Vector2(0.0f, 0.0f));
    texcoords.push_back(Vector2(1.0f, 0.0f));
    texcoords.push_back(Vector2(1.0f, 1.0f));
    texcoords.push_back(Vector2(0.0f, 1.0f));

    // 왼쪽
    positions.push_back(Vector3(-1.0f, -1.0f, 1.0f) * scale);
    positions.push_back(Vector3(-1.0f, 1.0f, 1.0f) * scale);
    positions.push_back(Vector3(-1.0f, 1.0f, -1.0f) * scale);
    positions.push_back(Vector3(-1.0f, -1.0f, -1.0f) * scale);
    colors.push_back(Vector3(1.0f, 1.0f, 0.0f));
    colors.push_back(Vector3(1.0f, 1.0f, 0.0f));
    colors.push_back(Vector3(1.0f, 1.0f, 0.0f));
    colors.push_back(Vector3(1.0f, 1.0f, 0.0f));
    normals.push_back(Vector3(-1.0f, 0.0f, 0.0f));
    normals.push_back(Vector3(-1.0f, 0.0f, 0.0f));
    normals.push_back(Vector3(-1.0f, 0.0f, 0.0f));
    normals.push_back(Vector3(-1.0f, 0.0f, 0.0f));
    texcoords.push_back(Vector2(0.0f, 0.0f));
    texcoords.push_back(Vector2(1.0f, 0.0f));
    texcoords.push_back(Vector2(1.0f, 1.0f));
    texcoords.push_back(Vector2(0.0f, 1.0f));

    // 오른쪽
    positions.push_back(Vector3(1.0f, -1.0f, 1.0f) * scale);
    positions.push_back(Vector3(1.0f, -1.0f, -1.0f) * scale);
    positions.push_back(Vector3(1.0f, 1.0f, -1.0f) * scale);
    positions.push_back(Vector3(1.0f, 1.0f, 1.0f) * scale);
    colors.push_back(Vector3(1.0f, 0.0f, 1.0f));
    colors.push_back(Vector3(1.0f, 0.0f, 1.0f));
    colors.push_back(Vector3(1.0f, 0.0f, 1.0f));
    colors.push_back(Vector3(1.0f, 0.0f, 1.0f));
    normals.push_back(Vector3(1.0f, 0.0f, 0.0f));
    normals.push_back(Vector3(1.0f, 0.0f, 0.0f));
    normals.push_back(Vector3(1.0f, 0.0f, 0.0f));
    normals.push_back(Vector3(1.0f, 0.0f, 0.0f));
    texcoords.push_back(Vector2(0.0f, 0.0f));
    texcoords.push_back(Vector2(1.0f, 0.0f));
    texcoords.push_back(Vector2(1.0f, 1.0f));
    texcoords.push_back(Vector2(0.0f, 1.0f));

    MeshData meshData;
    for (size_t i = 0; i < positions.size(); i++) {
        Vertex v;
        v.position  = positions[i];
        v.color = colors[i];
        v.normal    = normals[i];
        v.texcoord  = texcoords[i];
        meshData.vertices.push_back(v);
    }

    meshData.indices = {
        0,  1,  2,  0,  2,  3,  // 윗면
        4,  5,  6,  4,  6,  7,  // 아랫면
        8,  9,  10, 8,  10, 11, // 앞면
        12, 13, 14, 12, 14, 15, // 뒷면
        16, 17, 18, 16, 18, 19, // 왼쪽
        20, 21, 22, 20, 22, 23  // 오른쪽
    };

    return meshData;
}

std::vector<MeshData> GeometryGenerator::ReadFromFile(std::string filePath) {

    using namespace DirectX;

    ModelLoader modelLoader;
    modelLoader.Load(filePath);
    std::vector<MeshData>& meshes = modelLoader.meshes;

    // Normalize vertices
    Vector3 vmin(1000, 1000, 1000);
    Vector3 vmax(-1000, -1000, -1000);
    for (auto& mesh : meshes) {
        for (auto& v : mesh.vertices) {
            vmin.x = XMMin(vmin.x, v.position.x);
            vmin.y = XMMin(vmin.y, v.position.y);
            vmin.z = XMMin(vmin.z, v.position.z);
            vmax.x = XMMax(vmax.x, v.position.x);
            vmax.y = XMMax(vmax.y, v.position.y);
            vmax.z = XMMax(vmax.z, v.position.z);
        }
    }

    float dx = vmax.x - vmin.x, dy = vmax.y - vmin.y, dz = vmax.z - vmin.z;
    float dl = XMMax(XMMax(dx, dy), dz);
    float cx = (vmax.x + vmin.x) * 0.5f, cy = (vmax.y + vmin.y) * 0.5f,
        cz = (vmax.z + vmin.z) * 0.5f;

    for (auto& mesh : meshes) {
        for (auto& v : mesh.vertices) {
            v.position.x = (v.position.x - cx) / dl;
            v.position.y = (v.position.y - cy) / dl;
            v.position.z = (v.position.z - cz) / dl;
        }
    }

    return meshes;
}